// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <xcscheme/xcscheme.h>
#include <xcworkspace/xcworkspace.h>
#include <pbxbuild/pbxbuild.h>
#include <ios>
#include <fstream>

int
main(int argc, char **argv)
{
    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    if (argc < 5) {
        printf("Usage: %s workspace scheme config action\n", argv[0]);
        return -1;
    }

    xcworkspace::XC::Workspace::shared_ptr workspace = xcworkspace::XC::Workspace::Open(argv[1]);
    if (workspace == nullptr) {
        fprintf(stderr, "failed opening workspace\n");
        return -1;
    }

    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(workspace->projectFile(), workspace->name());
    if (group == nullptr) {
        fprintf(stderr, "failed opening scheme\n");
        return -1;
    }

    xcscheme::XC::Scheme::shared_ptr scheme = nullptr;
    for (xcscheme::XC::Scheme::shared_ptr const &available : group->schemes()) {
        if (available->name() == argv[2]) {
            scheme = available;
            break;
        }
    }
    if (scheme == nullptr) {
        fprintf(stderr, "couldn't find scheme\n");
        return -1;
    }

    pbxbuild::WorkspaceContext workspaceContext = pbxbuild::WorkspaceContext::Workspace(workspace);
    pbxbuild::BuildContext buildContext = pbxbuild::BuildContext::Create(
        workspaceContext,
        scheme,
        argv[3],
        argv[4],
        false,
        {
            pbxsetting::Level({
                pbxsetting::Setting::Parse("SDKROOT", "iphonesimulator"),
                pbxsetting::Setting::Parse("ARCHS", "i386"),
            }),
        }
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    auto graph = resolver.resolveDependencies(buildContext);
    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    bool color = isatty(fileno(stdout));
    std::shared_ptr<pbxbuild::Build::DefaultFormatter> formatter = pbxbuild::Build::DefaultFormatter::Create(color);
    pbxbuild::Build::Formatter::Print(formatter->begin(buildContext));

    std::unique_ptr<pbxbuild::Build::SimpleExecutor> executor = pbxbuild::Build::SimpleExecutor::Create(*buildEnvironment, buildContext, std::static_pointer_cast<pbxbuild::Build::Formatter>(formatter), false);

    bool succeeded = true;
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        pbxbuild::Build::Formatter::Print(formatter->beginTarget(buildContext, target));
        std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = buildContext.targetEnvironment(*buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment\n");
            continue;
        }

        pbxbuild::Build::Formatter::Print(formatter->checkDependencies(target));
        pbxbuild::Phase::PhaseContext phaseContext = pbxbuild::Phase::PhaseContext(*buildEnvironment, buildContext, target, *targetEnvironment);
        pbxbuild::Phase::PhaseInvocations phaseInvocations = pbxbuild::Phase::PhaseInvocations::Create(phaseContext, target);

        if (!executor->buildTarget(target, *targetEnvironment, phaseInvocations.orderedPhases(), phaseInvocations.invocations())) {
            return 1;
        }
    }

    pbxbuild::Build::Formatter::Print(formatter->success(buildContext));
    return 0;
}
