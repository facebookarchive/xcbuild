/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/BuildAction.h>
#include <xcdriver/Action.h>
#include <xcdriver/Options.h>
#include <builtin/builtin.h>

using xcdriver::BuildAction;
using xcdriver::Options;
using libutil::FSUtil;

BuildAction::
BuildAction()
{
}

BuildAction::
~BuildAction()
{
}

static std::shared_ptr<pbxbuild::Action::Formatter>
CreateFormatter(std::string const &formatter)
{
    if (formatter == "default" || formatter.empty()) {
        /* Only use color if attached to a terminal. */
        bool color = isatty(fileno(stdout));

        auto formatter = pbxbuild::Action::DefaultFormatter::Create(color);
        return std::static_pointer_cast<pbxbuild::Action::Formatter>(formatter);
    }

    return nullptr;
}

static std::unique_ptr<pbxbuild::Action::Executor>
CreateExecutor(
    std::string const &executor,
    std::shared_ptr<pbxbuild::Action::Formatter> const &formatter,
    bool dryRun)
{
    if (executor == "simple" || executor.empty()) {
        auto registry = builtin::Registry::Default();
        auto executor = pbxbuild::Action::SimpleExecutor::Create(formatter, dryRun, registry);
        return libutil::static_unique_pointer_cast<pbxbuild::Action::Executor>(std::move(executor));
    } else if (executor == "ninja") {
        auto executor = pbxbuild::Action::NinjaExecutor::Create(formatter, dryRun);
        return libutil::static_unique_pointer_cast<pbxbuild::Action::Executor>(std::move(executor));
    }

    return nullptr;
}

static bool
VerifySupportedOptions(Options const &options)
{
    if (!options.toolchain().empty()) {
        fprintf(stderr, "warning: toolchain option not implemented\n");
    }

    if (!options.destination().empty() || !options.destinationTimeout().empty()) {
        fprintf(stderr, "warning: destination option not implemented\n");
    }

    if (options.parallelizeTargets() || options.jobs() > 0) {
        fprintf(stderr, "warning: job control option not implemented\n");
    }

    if (options.hideShellScriptEnvironment()) {
        fprintf(stderr, "warning: output control option not implemented\n");
    }

    if (options.enableAddressSanitizer() || options.enableCodeCoverage()) {
        fprintf(stderr, "warning: build mode option not implemented\n");
    }

    if (!options.derivedDataPath().empty()) {
        fprintf(stderr, "warning: custom derived data path not implemented\n");
    }

    if (!options.resultBundlePath().empty()) {
        fprintf(stderr, "warning: result bundle path not implemented\n");
    }

    return true;
}

int BuildAction::
Run(Options const &options)
{
    // TODO(grp): Implement these options.
    if (!VerifySupportedOptions(options)) {
        return -1;
    }

    /* Verify the build options are not conflicting or invalid. */
    if (!Action::VerifyBuildActions(options.actions())) {
        return -1;
    }

    /*
     * Use the default build environment. We don't need anything custom here.
     */
    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    /*
     * Load the workspace for the provided options. There may or may not be an actual workspace;
     * the workspace context abstracts either a single project or a workspace.
     */
    std::unique_ptr<pbxbuild::WorkspaceContext> workspaceContext = Action::CreateWorkspace(options);
    if (workspaceContext == nullptr) {
        return -1;
    }

    /* The build settings passed in on the command line override all others. */
    std::vector<pbxsetting::Level> overrideLevels = Action::CreateOverrideLevels(options, buildEnvironment->baseEnvironment());

    /*
     * Create the build context for builing a specific scheme in the workspace.
     */
    std::unique_ptr<pbxbuild::BuildContext> buildContext = Action::CreateBuildContext(options, *workspaceContext, overrideLevels);
    if (buildContext == nullptr) {
        return -1;
    }

    /*
     * Build the target dependency graph. The executor uses this to know which targets to build.
     */
    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    pbxbuild::BuildGraph<pbxproj::PBX::Target::shared_ptr> graph;
    if (buildContext->scheme() != nullptr) {
        graph = resolver.resolveSchemeDependencies(*buildContext);
    } else if (workspaceContext->project() != nullptr) {
        graph = resolver.resolveLegacyDependencies(*buildContext, options.allTargets(), options.target());
    } else {
        fprintf(stderr, "error: scheme is required for workspace\n");
        return -1;
    }

    /*
     * Create the formatter to format the build log.
     */
    std::shared_ptr<pbxbuild::Action::Formatter> formatter = CreateFormatter(options.formatter());
    if (formatter == nullptr) {
        fprintf(stderr, "error: unknown formatter %s\n", options.formatter().c_str());
        return -1;
    }

    /*
     * Create the executor used to perform the build.
     */
    std::unique_ptr<pbxbuild::Action::Executor> executor = CreateExecutor(options.executor(), formatter, options.dryRun());
    if (executor == nullptr) {
        fprintf(stderr, "error: unknown executor %s\n", options.executor().c_str());
        return -1;
    }

    /*
     * Perform the build!
     */
    bool success = executor->build(*buildEnvironment, *buildContext, graph);
    if (!success) {
        return 1;
    }

    return 0;
}
