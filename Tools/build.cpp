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
    if (argc < 5) {
        printf("Usage: %s workspace scheme config action\n", argv[0]);
        return 1;
    }

    xcworkspace::XC::Workspace::shared_ptr workspace = xcworkspace::XC::Workspace::Open(argv[1]);
    if (workspace == nullptr) {
        fprintf(stderr, "failed opening workspace\n");
        return 1;
    }

    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(workspace->projectFile(), workspace->name());
    if (group == nullptr) {
        fprintf(stderr, "failed opening scheme\n");
        return 1;
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
        return 1;
    }

    std::string developerRoot = xcsdk::Environment::DeveloperRoot();
    std::shared_ptr<xcsdk::SDK::Manager> xcsdk_manager = xcsdk::SDK::Manager::Open(developerRoot);
    if (!xcsdk_manager) {
        fprintf(stderr, "no developer dir found\n");
        return -1;
    }

    std::string specificationRoot = pbxspec::Manager::SpecificationRoot(developerRoot);
    auto spec_manager = pbxspec::Manager::Open(nullptr, specificationRoot);
    if (!spec_manager) {
        fprintf(stderr, "error opening specifications\n");
        return -1;
    }

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = spec_manager->buildSystem("com.apple.build-system.core");

    auto platform = *std::find_if(xcsdk_manager->platforms().begin(), xcsdk_manager->platforms().end(), [](std::shared_ptr<xcsdk::SDK::Platform> platform) -> bool {
        return platform->name() == "iphoneos";
    });
    auto sdk = platform->targets().front();

    // NOTE(grp): Some platforms have specifications in other directories besides the primary Specifications folder.
    auto platformSpecifications = pbxspec::Manager::Open(spec_manager, platform->path() + "/Developer/Library/Xcode");

    pbxspec::PBX::Architecture::vector architectures = platformSpecifications->architectures();
    std::vector<pbxsetting::Setting> architectureSettings;
    std::vector<std::string> platformArchitectures;
    for (pbxspec::PBX::Architecture::shared_ptr architecture : architectures) {
        if (!architecture->architectureSetting().empty()) {
            architectureSettings.push_back(architecture->defaultSetting());
        }
        if (architecture->realArchitectures().empty()) {
            if (std::find(platformArchitectures.begin(), platformArchitectures.end(), architecture->identifier()) == platformArchitectures.end()) {
                platformArchitectures.push_back(architecture->identifier());
            }
        }
    }
    std::string platformArchitecturesValue;
    for (std::string const &arch : platformArchitectures) {
        if (&arch != &platformArchitectures[0]) {
            platformArchitecturesValue += " ";
        }
        platformArchitecturesValue += arch;
    }
    architectureSettings.push_back(pbxsetting::Setting::Parse("VALID_ARCHS", platformArchitecturesValue));
    pbxsetting::Level architectureLevel = pbxsetting::Level(architectureSettings);

    std::vector<pbxsetting::Level> base_levels;

    base_levels.push_back(platform->overrideProperties());
    base_levels.push_back(sdk->customProperties());
    base_levels.push_back(sdk->settings());
    base_levels.push_back(platform->settings());
    base_levels.push_back(sdk->defaultProperties());
    base_levels.push_back(architectureLevel);
    base_levels.push_back(platform->defaultProperties());
    base_levels.push_back(xcsdk_manager->computedSettings());

    std::vector<pbxsetting::Level> defaultLevels = pbxsetting::DefaultSettings::Levels();
    base_levels.insert(base_levels.end(), defaultLevels.begin(), defaultLevels.end());

    base_levels.push_back(buildSystem->defaultSettings());

    pbxsetting::Environment base_environment = pbxsetting::Environment(base_levels, base_levels);

    pbxbuild::SchemeContext::shared_ptr context = pbxbuild::SchemeContext::Create(
        argv[3],
        argv[4],
        scheme,
        workspace,
        base_environment
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(context);
    pbxbuild::BuildGraph graph = resolver.resolveDependencies();
    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    printf("Targets:\n");
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        printf("\t%s\n", target->name().c_str());
    }
}
