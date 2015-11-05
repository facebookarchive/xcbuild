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

    if (!Action::VerifyBuildActions(options.actions())) {
        return -1;
    }

    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    std::unique_ptr<pbxbuild::WorkspaceContext> workspaceContext = Action::CreateWorkspace(options);
    if (workspaceContext == nullptr) {
        return -1;
    }

    std::vector<pbxsetting::Level> overrideLevels = Action::CreateOverrideLevels(options, buildEnvironment->baseEnvironment());

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

    bool color = isatty(fileno(stdout));
    std::shared_ptr<pbxbuild::Build::DefaultFormatter> formatter = pbxbuild::Build::DefaultFormatter::Create(color);
    pbxbuild::Build::Formatter::Print(formatter->begin(*buildContext));

    std::unique_ptr<pbxbuild::Build::SimpleExecutor> executor = pbxbuild::Build::SimpleExecutor::Create(
        *buildEnvironment,
        *buildContext,
        std::static_pointer_cast<pbxbuild::Build::Formatter>(formatter),
        options.dryRun()
    );

    bool succeeded = true;
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        pbxbuild::Build::Formatter::Print(formatter->beginTarget(*buildContext, target));
        std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = buildContext->targetEnvironment(*buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment\n");
            continue;
        }

        pbxbuild::Build::Formatter::Print(formatter->checkDependencies(target));
        pbxbuild::Phase::PhaseContext phaseContext = pbxbuild::Phase::PhaseContext(*buildEnvironment, *buildContext, target, *targetEnvironment);
        pbxbuild::Phase::PhaseInvocations phaseInvocations = pbxbuild::Phase::PhaseInvocations::Create(phaseContext, target);

        if (!executor->buildTarget(target, *targetEnvironment, phaseInvocations.orderedPhases(), phaseInvocations.invocations())) {
            return 1;
        }
    }

    pbxbuild::Build::Formatter::Print(formatter->success(*buildContext));
    return 0;
}
