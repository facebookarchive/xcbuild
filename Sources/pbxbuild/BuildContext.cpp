// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildContext.h>

using pbxbuild::BuildContext;

BuildContext::
BuildContext(pbxspec::Manager::shared_ptr specManager, std::shared_ptr<xcsdk::SDK::Manager> sdkManager, pbxsetting::Environment baseEnvironment) :
    _specManager(specManager),
    _sdkManager(sdkManager),
    _baseEnvironment(baseEnvironment)
{
}

BuildContext::shared_ptr BuildContext::
Create(pbxspec::Manager::shared_ptr specManager, std::shared_ptr<xcsdk::SDK::Manager> sdkManager, pbxsetting::Environment baseEnvironment)
{
    return std::make_shared<BuildContext>(BuildContext(specManager, sdkManager, baseEnvironment));
}

BuildContext::shared_ptr BuildContext::
Default(void)
{
    std::string developerRoot = xcsdk::Environment::DeveloperRoot();
    std::shared_ptr<xcsdk::SDK::Manager> sdkManager = xcsdk::SDK::Manager::Open(developerRoot);
    if (sdkManager == nullptr) {
        return nullptr;
    }

    std::string specificationRoot = pbxspec::Manager::SpecificationRoot(developerRoot);
    auto specManager = pbxspec::Manager::Open(nullptr, specificationRoot);
    if (specManager == nullptr) {
        return nullptr;
    }

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = specManager->buildSystem("com.apple.build-system.core");
    if (buildSystem == nullptr) {
        return nullptr;
    }

    std::vector<pbxsetting::Level> levels;
    levels.push_back(sdkManager->computedSettings());
    std::vector<pbxsetting::Level> defaultLevels = pbxsetting::DefaultSettings::Levels();
    levels.insert(levels.end(), defaultLevels.begin(), defaultLevels.end());
    levels.push_back(buildSystem->defaultSettings());
    pbxsetting::Environment baseEnvironment = pbxsetting::Environment(levels, levels);

    return Create(specManager, sdkManager, baseEnvironment);
}
