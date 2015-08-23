// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/TargetEnvironment.h>

using pbxbuild::TargetEnvironment;

TargetEnvironment::
TargetEnvironment(pbxbuild::BuildContext::shared_ptr const &buildContext) :
    _buildContext(buildContext)
{
}

TargetEnvironment::
~TargetEnvironment()
{
}

static pbxproj::XC::BuildConfiguration::shared_ptr
ConfigurationNamed(pbxproj::XC::ConfigurationList::shared_ptr const &configurationList, std::string const &configuration)
{
    if (configurationList == nullptr) {
        return nullptr;
    }

    auto configurationIterator = std::find_if(configurationList->begin(), configurationList->end(), [&](pbxproj::XC::BuildConfiguration::shared_ptr buildConfiguration) -> bool {
        return buildConfiguration->name() == configuration;
    });

    if (configurationIterator == configurationList->end()) {
        return nullptr;
    }

    return *configurationIterator;
}

static pbxsetting::XC::Config::shared_ptr
LoadConfigurationFile(pbxproj::XC::BuildConfiguration::shared_ptr const &buildConfiguration, pbxsetting::Environment const &environment)
{
    if (buildConfiguration->baseConfigurationReference() == nullptr) {
        return nullptr;
    }

    pbxsetting::Value configurationValue = buildConfiguration->baseConfigurationReference()->resolve();
    std::string configurationPath = environment.expand(configurationValue);
    return pbxsetting::XC::Config::Open(configurationPath, environment);
}

static pbxsetting::Level
PlatformArchitecturesLevel(pbxspec::Manager::shared_ptr const &platformSpecifications)
{
    std::vector<pbxsetting::Setting> architectureSettings;
    std::vector<std::string> platformArchitectures;

    pbxspec::PBX::Architecture::vector architectures = platformSpecifications->architectures();
    for (pbxspec::PBX::Architecture::shared_ptr const &architecture : architectures) {
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

    return pbxsetting::Level(architectureSettings);
}

static xcsdk::SDK::Target::shared_ptr
FindPlatformTarget(std::shared_ptr<xcsdk::SDK::Manager> const &sdkManager, std::string const &sdkroot)
{
    for (xcsdk::SDK::Platform::shared_ptr const &platform : sdkManager->platforms()) {
        for (xcsdk::SDK::Target::shared_ptr const &sdk : platform->targets()) {
            if (sdk->canonicalName() == sdkroot || sdk->path() == sdkroot) {
                return sdk;
            }
        }
    }

    return nullptr;
}

static pbxsetting::Level
PackageProductTypeLevel(pbxspec::Manager::shared_ptr const &platformSpecifications, pbxproj::PBX::Target::shared_ptr const &target)
{
    pbxproj::PBX::NativeTarget *nativeTarget = reinterpret_cast<pbxproj::PBX::NativeTarget *>(target.get());

    pbxspec::PBX::ProductType::shared_ptr productType = platformSpecifications->productType(nativeTarget->productType());
    // FIXME(grp): Should this always use the first package type?
    pbxspec::PBX::PackageType::shared_ptr packageType = platformSpecifications->packageType(productType->packageTypes().at(0));

    std::vector<pbxsetting::Setting> settings = {
        pbxsetting::Setting::Parse("PACKAGE_TYPE", packageType->identifier()),
        pbxsetting::Setting::Parse("PRODUCT_TYPE", productType->identifier()),
    };

    pbxsetting::Level productTypeLevel = productType->defaultBuildProperties();
    settings.insert(settings.end(), productTypeLevel.settings().begin(), productTypeLevel.settings().end());

    pbxsetting::Level packageTypeLevel = packageType->defaultBuildSettings();
    settings.insert(settings.end(), packageTypeLevel.settings().begin(), packageTypeLevel.settings().end());

    return pbxsetting::Level(settings);
}

static pbxspec::PBX::BuildSystem::shared_ptr
TargetBuildSystem(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        return specManager->buildSystem("com.apple.build-system.native");
    } else if (target->type() == pbxproj::PBX::Target::kTypeLegacy) {
        return specManager->buildSystem("com.apple.build-system.jam");
    // } else if (target->type() == pbxproj::PBX::Target::kTypeAggregate) {
    //    return specManager->buildSystem("com.apple.build-system.external");
    } else {
        fprintf(stderr, "error: unknown target type\n");
        return nullptr;
    }
}

std::unique_ptr<pbxsetting::Environment> TargetEnvironment::
targetEnvironment(pbxproj::PBX::Target::shared_ptr const &target, std::string const &configuration) const
{
    std::vector<pbxsetting::Level> defaultLevels = _buildContext->baseEnvironment().assignment();

    pbxproj::XC::BuildConfiguration::shared_ptr projectConfiguration = ConfigurationNamed(target->project()->buildConfigurationList(), configuration);
    if (projectConfiguration == nullptr) {
        return nullptr;
    }

    // FIXME(grp): $(SRCROOT) must be set in order to find the xcconfig, but we need the xcconfig to know $(SDKROOT). So this can't
    // use the default level order, because $(SRCROOT) comes below $(SDKROOT). Hack around this for now with a synthetic environment.
    // It's also in the wrong order because project settings should be below the xcconfig, but are needed to *load* the xcconfig.
    std::vector<pbxsetting::Level> projectLevels;
    projectLevels.push_back(projectConfiguration->buildSettings());
    projectLevels.push_back(target->project()->settings());
    projectLevels.insert(projectLevels.end(), defaultLevels.begin(), defaultLevels.end());
    pbxsetting::Environment projectEnvironment = pbxsetting::Environment(projectLevels, projectLevels);

    pbxsetting::XC::Config::shared_ptr projectConfigurationFile = LoadConfigurationFile(projectConfiguration, projectEnvironment);
    if (projectConfigurationFile != nullptr) {
        projectLevels.insert(projectLevels.begin(), projectConfigurationFile->level());
    }

    pbxproj::XC::BuildConfiguration::shared_ptr targetConfiguration = ConfigurationNamed(target->buildConfigurationList(), configuration);
    if (targetConfiguration == nullptr) {
        return nullptr;
    }

    // FIXME(grp): Similar issue for the target xcconfig. These levels aren't complete (no platform) but are needed to *get* which SDK to use.
    std::vector<pbxsetting::Level> targetLevels = projectLevels;
    targetLevels.push_back(targetConfiguration->buildSettings());
    targetLevels.push_back(target->settings());
    targetLevels.insert(targetLevels.end(), projectLevels.begin(), projectLevels.end());
    targetLevels.insert(targetLevels.end(), defaultLevels.begin(), defaultLevels.end());
    pbxsetting::Environment targetEnvironment = pbxsetting::Environment(targetLevels, targetLevels);

    pbxsetting::XC::Config::shared_ptr targetConfigurationFile = LoadConfigurationFile(targetConfiguration, projectEnvironment);
    if (targetConfigurationFile != nullptr) {
        targetLevels.insert(targetLevels.begin(), targetConfigurationFile->level());
    }

    std::string sdkroot = targetEnvironment.resolve("SDKROOT");
    xcsdk::SDK::Target::shared_ptr sdk = FindPlatformTarget(_buildContext->sdkManager(), sdkroot);
    if (sdk == nullptr) {
        return nullptr;
    }

    // NOTE(grp): Some platforms have specifications in other directories besides the primary Specifications folder.
    pbxspec::Manager::shared_ptr platformSpecifications = pbxspec::Manager::Open(_buildContext->specManager(), sdk->platform()->path() + "/Developer/Library/Xcode");
    if (platformSpecifications == nullptr) {
        return nullptr;
    }

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = TargetBuildSystem(_buildContext->specManager(), target);

    // Now we have $(SDKROOT), and can make the real levels.
    std::vector<pbxsetting::Level> levels;

    if (targetConfigurationFile != nullptr) {
        levels.push_back(targetConfigurationFile->level());
    }
    levels.push_back(targetConfiguration->buildSettings());
    levels.push_back(target->settings());

    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        levels.push_back(PackageProductTypeLevel(platformSpecifications, target));
    }

    if (projectConfigurationFile != nullptr) {
        levels.push_back(projectConfigurationFile->level());
    }
    levels.push_back(projectConfiguration->buildSettings());
    levels.push_back(target->project()->settings());

    // TODO(grp): Verify this ordering is correct.
    levels.push_back(sdk->platform()->overrideProperties());
    levels.push_back(sdk->customProperties());
    levels.push_back(sdk->settings());
    levels.push_back(sdk->platform()->settings());
    levels.push_back(sdk->defaultProperties());
    levels.push_back(PlatformArchitecturesLevel(platformSpecifications));
    levels.push_back(sdk->platform()->defaultProperties());

    levels.push_back(buildSystem->defaultSettings());

    levels.insert(levels.end(), defaultLevels.begin(), defaultLevels.end());
    return std::make_unique<pbxsetting::Environment>(pbxsetting::Environment(levels, levels));
}
