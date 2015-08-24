// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <xcscheme/xcscheme.h>
#include <xcworkspace/xcworkspace.h>
#include <pbxbuild/pbxbuild.h>

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
        printf("scheme: %s\n", available->name().c_str());
        if (available->name() == argv[2]) {
            scheme = available;
            break;
        }
    }
    if (scheme == nullptr) {
        fprintf(stderr, "couldn't find scheme\n");
        return -1;
    }

    pbxbuild::SchemeContext context = pbxbuild::SchemeContext(
        scheme,
        workspace,
        argv[3],
        argv[4]
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    pbxbuild::BuildGraph graph = resolver.resolveDependencies(context);
    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    printf("Targets:\n");
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        printf("\t%s\n", target->name().c_str());
    }
}
