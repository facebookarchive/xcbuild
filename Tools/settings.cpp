// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s project\n", argv[0]);
        return -1;
    }

    std::string developerRoot = xcsdk::Environment::DeveloperRoot();
    std::shared_ptr<xcsdk::SDK::Manager> xcsdk_manager = xcsdk::SDK::Manager::Open(developerRoot);
    if (!xcsdk_manager) {
        fprintf(stderr, "no sdks found\n");
        return -1;
    }

    auto project = pbxproj::PBX::Project::Open(argv[1]);
    if (!project) {
        fprintf(stderr, "error opening project at %s (%s)\n", argv[1], strerror(errno));
        return -1;
    }

    std::string specificationRoot = pbxspec::Manager::SpecificationRoot(developerRoot);
    auto spec_manager = pbxspec::Manager::Open(nullptr, specificationRoot);
    if (!spec_manager) {
        fprintf(stderr, "error opening specifications at %s (%s)\n", specificationRoot.c_str(), strerror(errno));
        return -1;
    }

    printf("Project: %s\n", project->name().c_str());

    auto projectConfigurationList = project->buildConfigurationList();
    std::string projectConfigurationName = projectConfigurationList->defaultConfigurationName();
    projectConfigurationName = projectConfigurationName.empty() ? "Release" : projectConfigurationName;
    auto projectConfiguration = *std::find_if(projectConfigurationList->begin(), projectConfigurationList->end(), [&](pbxproj::XC::BuildConfiguration::shared_ptr configuration) -> bool {
        return configuration->name() == projectConfigurationName;
    });
    printf("Project Configuration: %s\n", projectConfiguration->name().c_str());

    auto target = project->targets().front();
    printf("Target: %s\n", target->name().c_str());

    auto targetConfigurationList = target->buildConfigurationList();
    std::string targetConfigurationName = targetConfigurationList->defaultConfigurationName();
    targetConfigurationName = targetConfigurationName.empty() ? "Release" : targetConfigurationName;
    auto targetConfiguration = *std::find_if(targetConfigurationList->begin(), targetConfigurationList->end(), [&](pbxproj::XC::BuildConfiguration::shared_ptr configuration) -> bool {
        return configuration->name() == targetConfigurationName;
    });
    printf("Target Configuration: %s\n", targetConfiguration->name().c_str());

    auto platform = *std::find_if(xcsdk_manager->platforms().begin(), xcsdk_manager->platforms().end(), [](std::shared_ptr<xcsdk::SDK::Platform> platform) -> bool {
        return platform->name() == "iphoneos";
    });
    printf("Platform: %s\n", platform->name().c_str());

    // NOTE(grp): Some platforms have specifications in other directories besides the primary Specifications folder.
    auto platformSpecifications = pbxspec::Manager::Open(spec_manager, platform->path() + "/Developer/Library/Xcode");

    auto sdk = platform->targets().front();
    printf("SDK: %s\n", sdk->displayName().c_str());

    // TODO(grp): Handle legacy targets.
    assert(target->isa() == pbxproj::PBX::NativeTarget::Isa());
    pbxproj::PBX::NativeTarget::shared_ptr nativeTarget = std::dynamic_pointer_cast<pbxproj::PBX::NativeTarget>(target);

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = spec_manager->buildSystem("com.apple.build-system.native");
    pbxspec::PBX::ProductType::shared_ptr productType = platformSpecifications->productType(nativeTarget->productType());
    // TODO(grp): Should this always use the first package type?
    pbxspec::PBX::PackageType::shared_ptr packageType = platformSpecifications->packageType(productType->packageTypes().at(0));

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

    pbxsetting::Level config = pbxsetting::Level({
        pbxsetting::Setting::Parse("ACTION", "build"),
        pbxsetting::Setting::Parse("CONFIGURATION", targetConfigurationName),
        pbxsetting::Setting::Parse("CURRENT_ARCH", "arm64"), // TODO(grp): Should intersect VALID_ARCHS and ARCHS?
        pbxsetting::Setting::Parse("CURRENT_VARIANT", "normal"),
        pbxsetting::Setting::Parse("BUILD_COMPONENTS", "headers build"),
    });

    pbxsetting::Level targetSpecificationSettings = pbxsetting::Level({
        pbxsetting::Setting::Parse("PACKAGE_TYPE", packageType->identifier()),
        pbxsetting::Setting::Parse("PRODUCT_TYPE", productType->identifier()),
    });


    std::vector<pbxsetting::Level> base_levels;

    base_levels.push_back(projectConfiguration->buildSettings());
    base_levels.push_back(project->settings());

    base_levels.push_back(platform->overrideProperties());
    base_levels.push_back(sdk->customProperties());
    base_levels.push_back(sdk->settings());
    base_levels.push_back(platform->settings());
    base_levels.push_back(sdk->defaultProperties());
    base_levels.push_back(platform->defaultProperties());
    base_levels.push_back(xcsdk_manager->computedSettings());

    base_levels.push_back(pbxsetting::DefaultSettings::Environment());
    base_levels.push_back(pbxsetting::DefaultSettings::Internal());
    base_levels.push_back(pbxsetting::DefaultSettings::Local());
    base_levels.push_back(pbxsetting::DefaultSettings::System());
    base_levels.push_back(pbxsetting::DefaultSettings::Architecture());
    base_levels.push_back(pbxsetting::DefaultSettings::Build());

    base_levels.push_back(architectureLevel);
    base_levels.push_back(buildSystem->defaultSettings());

    pbxsetting::Environment base_environment = pbxsetting::Environment(base_levels, base_levels);


    std::vector<pbxsetting::Level> levels;
    levels.push_back(config);

    if (targetConfiguration->baseConfigurationReference() != nullptr) {
        pbxsetting::Value targetConfigurationValue = targetConfiguration->baseConfigurationReference()->resolve();
        std::string targetConfigurationPath = base_environment.expand(targetConfigurationValue);
        pbxsetting::XC::Config::shared_ptr targetConfigurationFile = pbxsetting::XC::Config::Open(targetConfigurationPath, base_environment);
        if (targetConfigurationFile != nullptr) {
            levels.push_back(targetConfigurationFile->level());
        }
    }
    levels.push_back(targetConfiguration->buildSettings());
    levels.push_back(target->settings());

    levels.push_back(targetSpecificationSettings);
    levels.push_back(packageType->defaultBuildSettings());
    levels.push_back(productType->defaultBuildProperties());

    if (projectConfiguration->baseConfigurationReference() != nullptr) {
        pbxsetting::Value projectConfigurationValue = projectConfiguration->baseConfigurationReference()->resolve();
        std::string projectConfigurationPath = base_environment.expand(projectConfigurationValue);
        pbxsetting::XC::Config::shared_ptr projectConfigurationFile = pbxsetting::XC::Config::Open(projectConfigurationPath, base_environment);
        if (projectConfigurationFile != nullptr) {
            levels.push_back(projectConfigurationFile->level());
        }
    }

    // TODO(grp): Figure out how these should be specified -- workspaces?
    // TODO(grp): Fix which settings are printed at the end -- appears to be ones with any override? But what about SED?
    levels.push_back(pbxsetting::Level({
        pbxsetting::Setting::Parse("CONFIGURATION_BUILD_DIR", "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),
        pbxsetting::Setting::Parse("CONFIGURATION_TEMP_DIR", "$(PROJECT_TEMP_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),

        // HACK(grp): Hardcode a few paths for testing.
        pbxsetting::Setting::Parse("SYMROOT", "$(DERIVED_DATA_DIR)/$(PROJECT_NAME)-cpmowfgmqamrjrfvwivglsgfzkff/Build/Products"),
        pbxsetting::Setting::Parse("OBJROOT", "$(DERIVED_DATA_DIR)/$(PROJECT_NAME)-cpmowfgmqamrjrfvwivglsgfzkff/Build/Intermediates"),
    }));

    levels.insert(levels.end(), base_environment.assignment().begin(), base_environment.assignment().end());
    pbxsetting::Environment environment = pbxsetting::Environment(levels, levels);


    pbxsetting::Condition condition = pbxsetting::Condition({
        { "sdk", sdk->canonicalName() },
        { "arch", environment.resolve("CURRENT_ARCH") },
        { "variant", environment.resolve("CURRENT_VARIANT") },
    });

    std::unordered_map<std::string, std::string> values = environment.computeValues(condition);
    std::map<std::string, std::string> orderedValues = std::map<std::string, std::string>(values.begin(), values.end());

    printf("\n\nBuild Settings:\n\n");
    for (auto const &value : orderedValues) {
        printf("    %s = %s\n", value.first.c_str(), value.second.c_str());
    }

    return 0;
}
