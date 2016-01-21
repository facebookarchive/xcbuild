/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Build/Environment.h>

namespace Build = pbxbuild::Build;

Build::Environment::
Environment(pbxspec::Manager::shared_ptr const &specManager, std::shared_ptr<xcsdk::SDK::Manager> const &sdkManager, pbxsetting::Environment const &baseEnvironment) :
    _specManager(specManager),
    _sdkManager(sdkManager),
    _baseEnvironment(baseEnvironment)
{
}

ext::optional<Build::Environment> Build::Environment::
Default(void)
{
    std::string developerRoot = xcsdk::Environment::DeveloperRoot();
    std::shared_ptr<xcsdk::SDK::Manager> sdkManager = xcsdk::SDK::Manager::Open(developerRoot);
    if (sdkManager == nullptr) {
        fprintf(stderr, "error: couldn't create SDK manager\n");
        return ext::nullopt;
    }

    auto specManager = pbxspec::Manager::Create();
    if (specManager == nullptr) {
        fprintf(stderr, "error: couldn't create spec manager\n");
        return ext::nullopt;
    }

    specManager->registerDomain(pbxspec::Manager::DefaultDomain(developerRoot));
    for (auto const &domain : pbxspec::Manager::EmbeddedDomains(developerRoot)) {
        specManager->registerDomain(domain);
    }
    for (auto const &domain : pbxspec::Manager::PlatformDomains(developerRoot)) {
        specManager->registerDomain(domain);
    }

    std::string buildRules = pbxspec::Manager::DeveloperBuildRules(developerRoot);
    specManager->registerBuildRules(buildRules);

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = specManager->buildSystem("com.apple.build-system.core", { "default" });
    if (buildSystem == nullptr) {
        fprintf(stderr, "error: couldn't create build system\n");
        return ext::nullopt;
    }

    pbxsetting::Environment baseEnvironment;
    baseEnvironment.insertBack(buildSystem->defaultSettings(), true);
    baseEnvironment.insertBack(sdkManager->computedSettings(), false);
    std::vector<pbxsetting::Level> defaultLevels = pbxsetting::DefaultSettings::Levels();
    for (pbxsetting::Level const &level : defaultLevels) {
        baseEnvironment.insertBack(level, false);
    }

    return Build::Environment(specManager, sdkManager, baseEnvironment);
}
