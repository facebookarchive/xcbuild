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

static std::shared_ptr<pbxbuild::Build::Formatter>
CreateFormatter(std::string const &formatter)
{
    if (formatter == "default" || formatter.empty()) {
        /* Only use color if attached to a terminal. */
        bool color = isatty(fileno(stdout));

        auto formatter = pbxbuild::Build::DefaultFormatter::Create(color);
        return std::static_pointer_cast<pbxbuild::Build::Formatter>(formatter);
    }

    return nullptr;
}

static std::unique_ptr<pbxbuild::Build::Executor>
CreateExecutor(
    std::string const &executor,
    pbxbuild::BuildEnvironment const &buildEnvironment,
    pbxbuild::BuildContext const &buildContext,
    std::shared_ptr<pbxbuild::Build::Formatter> const &formatter,
    bool dryRun)
{
    if (executor == "simple" || executor.empty()) {
        auto executor = pbxbuild::Build::SimpleExecutor::Create(
            buildEnvironment,
            buildContext,
            formatter,
            dryRun,
            builtin::Registry::Default()
        );
        return libutil::static_unique_pointer_cast<pbxbuild::Build::Executor>(std::move(executor));
    } else if (executor == "ninja") {
        auto executor = pbxbuild::Build::NinjaExecutor::Create(
            buildEnvironment,
            buildContext,
            formatter,
            dryRun
        );
        return libutil::static_unique_pointer_cast<pbxbuild::Build::Executor>(std::move(executor));
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

    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    /*
     * Create the formatter to format the build log.
     */
    std::shared_ptr<pbxbuild::Build::Formatter> formatter = CreateFormatter(options.formatter());
    if (formatter == nullptr) {
        fprintf(stderr, "error: unknown formatter %s\n", options.formatter().c_str());
        return -1;
    }

    /*
     * Create the executor used to perform the build.
     */
    std::unique_ptr<pbxbuild::Build::Executor> executor = CreateExecutor(options.executor(), *buildEnvironment, *buildContext, formatter, options.dryRun());
    if (executor == nullptr) {
        fprintf(stderr, "error: unknown executor %s\n", options.executor().c_str());
        return -1;
    }

    pbxbuild::Build::Formatter::Print(formatter->begin(*buildContext));

    bool succeeded = true;
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        pbxbuild::Build::Formatter::Print(formatter->beginTarget(*buildContext, target));

        std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = buildContext->targetEnvironment(*buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment\n");
            pbxbuild::Build::Formatter::Print(formatter->finishTarget(*buildContext, target));
            continue;
        }

        pbxbuild::Build::Formatter::Print(formatter->beginCheckDependencies(target));
        pbxbuild::Phase::PhaseEnvironment phaseEnvironment = pbxbuild::Phase::PhaseEnvironment(*buildEnvironment, *buildContext, target, *targetEnvironment);
        pbxbuild::Phase::PhaseInvocations phaseInvocations = pbxbuild::Phase::PhaseInvocations::Create(phaseEnvironment, target);
        pbxbuild::Build::Formatter::Print(formatter->finishCheckDependencies(target));

        if (!executor->buildTarget(target, *targetEnvironment, phaseInvocations.invocations())) {
            pbxbuild::Build::Formatter::Print(formatter->finishTarget(*buildContext, target));
            return 1;
        }

        pbxbuild::Build::Formatter::Print(formatter->finishTarget(*buildContext, target));
    }

    pbxbuild::Build::Formatter::Print(formatter->success(*buildContext));
    return 0;
}
