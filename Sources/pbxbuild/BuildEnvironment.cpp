// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildEnvironment.h>

using pbxbuild::BuildEnvironment;

BuildEnvironment::
BuildEnvironment(pbxspec::Manager::shared_ptr specManager, std::shared_ptr<xcsdk::SDK::Manager> sdkManager, pbxsetting::Environment baseEnvironment) :
    _specManager(specManager),
    _sdkManager(sdkManager),
    _baseEnvironment(baseEnvironment)
{
}

std::unique_ptr<BuildEnvironment> BuildEnvironment::
Default(void)
{
    std::string developerRoot = xcsdk::Environment::DeveloperRoot();
    std::shared_ptr<xcsdk::SDK::Manager> sdkManager = xcsdk::SDK::Manager::Open(developerRoot);
    if (sdkManager == nullptr) {
        return nullptr;
    }

    auto specManager = pbxspec::Manager::Create();
    if (specManager == nullptr) {
        return nullptr;
    }

    std::string specificationRoot = pbxspec::Manager::DeveloperSpecificationRoot(developerRoot);
    specManager->registerDomain(pbxspec::Manager::GlobalDomain(), specificationRoot);

    std::string buildRules = pbxspec::Manager::DeveloperBuildRules(developerRoot);
    specManager->registerBuildRules(buildRules);

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = specManager->buildSystem("com.apple.build-system.core");
    if (buildSystem == nullptr) {
        return nullptr;
    }

    pbxsetting::Environment baseEnvironment;
    baseEnvironment.insertBack(buildSystem->defaultSettings(), true);
    baseEnvironment.insertBack(sdkManager->computedSettings(), false);
    std::vector<pbxsetting::Level> defaultLevels = pbxsetting::DefaultSettings::Levels();
    for (pbxsetting::Level const &level : defaultLevels) {
        baseEnvironment.insertBack(level, false);
    }

    return std::make_unique<BuildEnvironment>(BuildEnvironment(specManager, sdkManager, baseEnvironment));
}
