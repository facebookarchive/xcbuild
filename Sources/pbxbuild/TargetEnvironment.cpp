// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/TargetEnvironment.h>

using pbxbuild::TargetEnvironment;
using pbxbuild::BuildEnvironment;
using pbxbuild::BuildContext;

TargetEnvironment::
TargetEnvironment(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const &context) :
    _buildEnvironment(buildEnvironment),
    _target(target),
    _context(context)
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
PlatformArchitecturesLevel(pbxspec::Manager::shared_ptr const &specManager, xcsdk::SDK::Target::shared_ptr const &sdk)
{
    std::vector<pbxsetting::Setting> architectureSettings;
    std::vector<std::string> platformArchitectures;

    pbxspec::PBX::Architecture::vector architectures = specManager->architectures(sdk->platform()->name());
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

        if (platform->name() == sdkroot || platform->path() == sdkroot) {
            return platform->targets().back();
        }
    }

    return nullptr;
}

static pbxsetting::Level
PackageProductTypeLevel(pbxspec::PBX::PackageType::shared_ptr const &packageType, pbxspec::PBX::ProductType::shared_ptr const &productType)
{
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
TargetBuildSystem(pbxspec::Manager::shared_ptr const &specManager, xcsdk::SDK::Target::shared_ptr const &sdk, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        return specManager->buildSystem("com.apple.build-system.native", sdk->platform()->name());
    } else if (target->type() == pbxproj::PBX::Target::kTypeLegacy) {
        return specManager->buildSystem("com.apple.build-system.jam", sdk->platform()->name());
    } else if (target->type() == pbxproj::PBX::Target::kTypeAggregate) {
       return specManager->buildSystem("com.apple.build-system.external", sdk->platform()->name());
    } else {
        fprintf(stderr, "error: unknown target type\n");
        return nullptr;
    }
}

std::unique_ptr<TargetEnvironment> TargetEnvironment::
Create(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const &context)
{
    std::vector<pbxsetting::Level> defaultLevels = buildEnvironment.baseEnvironment().assignment();

    pbxproj::XC::BuildConfiguration::shared_ptr projectConfiguration = ConfigurationNamed(target->project()->buildConfigurationList(), context.configuration());
    if (projectConfiguration == nullptr) {
        fprintf(stderr, "error: unable to find project configuration %s\n", context.configuration().c_str());
        return nullptr;
    }

    // FIXME(grp): $(SRCROOT) must be set in order to find the xcconfig, but we need the xcconfig to know $(SDKROOT). So this can't
    // use the default level order, because $(SRCROOT) comes below $(SDKROOT). Hack around this for now with a synthetic environment.
    // It's also in the wrong order because project settings should be below the xcconfig, but are needed to *load* the xcconfig.
    std::vector<pbxsetting::Level> projectLevels;
    projectLevels.push_back(context.actionSettings());
    projectLevels.push_back(projectConfiguration->buildSettings());
    projectLevels.push_back(target->project()->settings());
    projectLevels.push_back(context.baseSettings());
    projectLevels.insert(projectLevels.end(), defaultLevels.begin(), defaultLevels.end());
    pbxsetting::Environment projectEnvironment = pbxsetting::Environment(projectLevels, projectLevels);

    pbxsetting::XC::Config::shared_ptr projectConfigurationFile = LoadConfigurationFile(projectConfiguration, projectEnvironment);
    if (projectConfigurationFile != nullptr) {
        projectLevels.insert(projectLevels.begin(), projectConfigurationFile->level());
    }

    pbxproj::XC::BuildConfiguration::shared_ptr targetConfiguration = ConfigurationNamed(target->buildConfigurationList(), context.configuration());
    if (targetConfiguration == nullptr) {
        fprintf(stderr, "error: unable to find target configuration %s\n", context.configuration().c_str());
        return nullptr;
    }

    // FIXME(grp): Similar issue for the target xcconfig. These levels aren't complete (no platform) but are needed to *get* which SDK to use.
    std::vector<pbxsetting::Level> targetLevels = projectLevels;
    targetLevels.push_back(context.actionSettings());
    targetLevels.push_back(targetConfiguration->buildSettings());
    targetLevels.push_back(target->settings());
    targetLevels.insert(targetLevels.end(), projectLevels.begin(), projectLevels.end());
    targetLevels.push_back(context.baseSettings());
    targetLevels.insert(targetLevels.end(), defaultLevels.begin(), defaultLevels.end());
    pbxsetting::Environment targetEnvironment = pbxsetting::Environment(targetLevels, targetLevels);

    pbxsetting::XC::Config::shared_ptr targetConfigurationFile = LoadConfigurationFile(targetConfiguration, projectEnvironment);
    if (targetConfigurationFile != nullptr) {
        targetLevels.insert(targetLevels.begin(), targetConfigurationFile->level());
    }

    std::string sdkroot = targetEnvironment.resolve("SDKROOT");
    xcsdk::SDK::Target::shared_ptr sdk = FindPlatformTarget(buildEnvironment.sdkManager(), sdkroot);
    if (sdk == nullptr) {
        fprintf(stderr, "error: unable to find sdkroot %s\n", sdkroot.c_str());
        return nullptr;
    }

    std::string platformSpecificationPath = pbxspec::Manager::DomainSpecificationRoot(sdk->platform()->path());
    buildEnvironment.specManager()->registerDomain(sdk->platform()->name(), platformSpecificationPath);

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = TargetBuildSystem(buildEnvironment.specManager(), sdk, target);
    if (buildSystem == nullptr) {
        return nullptr;
    }

    pbxspec::PBX::ProductType::shared_ptr productType = nullptr;
    pbxspec::PBX::PackageType::shared_ptr packageType = nullptr;
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        pbxproj::PBX::NativeTarget *nativeTarget = reinterpret_cast<pbxproj::PBX::NativeTarget *>(target.get());

        productType = buildEnvironment.specManager()->productType(nativeTarget->productType(), sdk->platform()->name());
        if (productType == nullptr) {
            return nullptr;
        }

        // FIXME(grp): Should this always use the first package type?
        packageType = buildEnvironment.specManager()->packageType(productType->packageTypes().at(0), sdk->platform()->name());
        if (packageType == nullptr) {
            return nullptr;
        }
    }

    // Now we have $(SDKROOT), and can make the real levels.
    std::vector<pbxsetting::Level> levels;
    levels.push_back(context.actionSettings());

    if (targetConfigurationFile != nullptr) {
        levels.push_back(targetConfigurationFile->level());
    }
    levels.push_back(targetConfiguration->buildSettings());
    levels.push_back(target->settings());

    if (packageType != nullptr && productType != nullptr) {
        levels.push_back(PackageProductTypeLevel(packageType, productType));
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
    levels.push_back(PlatformArchitecturesLevel(buildEnvironment.specManager(), sdk));
    levels.push_back(sdk->platform()->defaultProperties());

    levels.push_back(context.baseSettings());
    levels.push_back(buildSystem->defaultSettings());

    levels.insert(levels.end(), defaultLevels.begin(), defaultLevels.end());
    pbxsetting::Environment environment = pbxsetting::Environment(levels, levels);

    std::unique_ptr<TargetEnvironment> te = std::make_unique<TargetEnvironment>(TargetEnvironment(buildEnvironment, target, context));
    te->_environment = std::make_unique<pbxsetting::Environment>(environment);
    te->_buildSystem = buildSystem;
    te->_packageType = packageType;
    te->_productType = productType;
    te->_sdk = sdk;
    return te;
}
