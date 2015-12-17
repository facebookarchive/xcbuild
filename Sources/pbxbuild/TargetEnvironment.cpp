/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/BuildContext.h>

using pbxbuild::TargetEnvironment;
using pbxbuild::BuildEnvironment;
using pbxbuild::BuildContext;
using libutil::FSUtil;

TargetEnvironment::
TargetEnvironment()
{
}

TargetEnvironment::
~TargetEnvironment()
{
}

static std::unordered_map<pbxproj::PBX::BuildFile::shared_ptr, std::string>
BuildFileDisambiguation(pbxproj::PBX::Target::shared_ptr const &target)
{
    std::unordered_map<std::string, pbxproj::PBX::BuildFile::shared_ptr> buildFileUnambiguous;
    std::unordered_map<pbxproj::PBX::BuildFile::shared_ptr, std::string> buildFileDisambiguation;

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
            std::string name = FSUtil::GetBaseNameWithoutExtension(buildFile->fileRef()->name());

            auto it = buildFileUnambiguous.find(name);
            if (it != buildFileUnambiguous.end()) {
                buildFileDisambiguation.insert({ it->second, it->first + "-" + it->second->blueprintIdentifier() });
                buildFileDisambiguation.insert({ buildFile, name + "-" + buildFile->blueprintIdentifier() });
            }
            buildFileUnambiguous.insert({ name, buildFile });
        }
    }

    return buildFileDisambiguation;
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

static std::vector<std::string>
SDKSpecificationDomains(xcsdk::SDK::Target::shared_ptr const &sdk)
{
    std::vector<std::string> domains;
    domains.push_back(sdk->platform()->name());

    // TODO(grp): Find a better way to determine what's embedded.
    if (sdk->platform()->name() != "macosx") {
        if (sdk->platform()->name().find("simulator") != std::string::npos) {
            domains.push_back("embedded-simulator");
        } else {
            domains.push_back("embedded");
        }

        domains.push_back("embedded-shared");
    }

    domains.push_back("default");
    return domains;
}

static pbxsetting::Level
PlatformArchitecturesLevel(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &specDomains)
{
    std::vector<pbxsetting::Setting> architectureSettings;
    std::vector<std::string> platformArchitectures;

    pbxspec::PBX::Architecture::vector architectures = specManager->architectures(specDomains);
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

static pbxsetting::Level
PackageTypeLevel(pbxspec::PBX::PackageType::shared_ptr const &packageType)
{
    std::vector<pbxsetting::Setting> settings = {
        pbxsetting::Setting::Parse("PACKAGE_TYPE", packageType->identifier()),
    };

    pbxsetting::Level packageTypeLevel = packageType->defaultBuildSettings();
    settings.insert(settings.end(), packageTypeLevel.settings().begin(), packageTypeLevel.settings().end());

    return pbxsetting::Level(settings);
}


static pbxsetting::Level
ProductTypeLevel(pbxspec::PBX::ProductType::shared_ptr const &productType)
{
    std::vector<pbxsetting::Setting> settings = {
        pbxsetting::Setting::Parse("PRODUCT_TYPE", productType->identifier()),
    };

    pbxsetting::Level productTypeLevel = productType->defaultBuildProperties();
    settings.insert(settings.end(), productTypeLevel.settings().begin(), productTypeLevel.settings().end());

    return pbxsetting::Level(settings);
}

static pbxspec::PBX::BuildSystem::shared_ptr
TargetBuildSystem(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &specDomains, pbxproj::PBX::Target::shared_ptr const &target)
{
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        return specManager->buildSystem("com.apple.build-system.native", specDomains);
    } else if (target->type() == pbxproj::PBX::Target::kTypeLegacy) {
        return specManager->buildSystem("com.apple.build-system.external", specDomains);
    } else if (target->type() == pbxproj::PBX::Target::kTypeAggregate) {
       return specManager->buildSystem("com.apple.build-system.external", specDomains);
    } else {
        fprintf(stderr, "error: unknown target type\n");
        return nullptr;
    }
}

static std::vector<std::string>
ResolveArchitectures(pbxsetting::Environment const &environment)
{
    std::vector<std::string> archsVector = pbxsetting::Type::ParseList(environment.resolve("ARCHS"));
    std::set<std::string> archs = std::set<std::string>(archsVector.begin(), archsVector.end());
    std::vector<std::string> validArchsVector = pbxsetting::Type::ParseList(environment.resolve("VALID_ARCHS"));
    std::set<std::string> validArchs = std::set<std::string>(validArchsVector.begin(), validArchsVector.end());

    std::vector<std::string> architectures;
    std::set_intersection(archs.begin(), archs.end(), validArchs.begin(), validArchs.end(), std::back_inserter(architectures));
    return architectures;
}

static std::vector<std::string>
ResolveVariants(pbxsetting::Environment const &environment)
{
    return pbxsetting::Type::ParseList(environment.resolve("BUILD_VARIANTS"));
}

static pbxsetting::Level
ArchitecturesVariantsLevel(std::vector<std::string> const &architectures, std::vector<std::string> const &variants)
{
    std::vector<pbxsetting::Setting> settings;

    if (!variants.empty()) {
        settings.push_back(pbxsetting::Setting::Parse("CURRENT_VARIANT", variants.front()));
        settings.push_back(pbxsetting::Setting::Parse("variant", variants.front()));
    }

    if (!architectures.empty()) {
        settings.push_back(pbxsetting::Setting::Parse("CURRENT_ARCH", architectures.front()));
        settings.push_back(pbxsetting::Setting::Parse("arch", architectures.front()));
    }

    for (std::string const &variant : variants) {
        pbxsetting::Setting objectFileDir = pbxsetting::Setting::Parse("OBJECT_FILE_DIR_" + variant, "$(OBJECT_FILE_DIR)-" + variant);
        settings.push_back(objectFileDir);

        for (std::string const &arch : architectures) {
            std::string linkFileList = "LINK_FILE_LIST_" + variant + "_" + arch;
            std::string linkFileListPath = "$(OBJECT_FILE_DIR_" + variant + ")/" + arch + "/$(PRODUCT_NAME).LinkFileList";
            settings.push_back(pbxsetting::Setting::Parse(linkFileList, linkFileListPath));
        }
    }

    return pbxsetting::Level(settings);
}

std::unique_ptr<TargetEnvironment> TargetEnvironment::
Create(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const *context)
{
    xcsdk::SDK::Target::shared_ptr sdk;
    std::vector<std::string> specDomains;
    pbxproj::XC::BuildConfiguration::shared_ptr projectConfiguration;
    pbxproj::XC::BuildConfiguration::shared_ptr targetConfiguration;
    pbxsetting::XC::Config::shared_ptr projectConfigurationFile;
    pbxsetting::XC::Config::shared_ptr targetConfigurationFile;
    {
        // FIXME(grp): $(SRCROOT) must be set in order to find the xcconfig, but we need the xcconfig to know $(SDKROOT). So this can't
        // use the default level order, because $(SRCROOT) comes below $(SDKROOT). Hack around this for now with a synthetic environment.
        // It's also in the wrong order because project settings should be below the SDK, but are needed to *load* the xcconfig.
        pbxsetting::Environment determinationEnvironment = buildEnvironment.baseEnvironment();
        determinationEnvironment.insertFront(context->baseSettings(), false);

        projectConfiguration = ConfigurationNamed(target->project()->buildConfigurationList(), context->configuration());
        if (projectConfiguration == nullptr) {
            fprintf(stderr, "error: unable to find project configuration %s\n", context->configuration().c_str());
            return nullptr;
        }

        determinationEnvironment.insertFront(target->project()->settings(), false);
        determinationEnvironment.insertFront(projectConfiguration->buildSettings(), false);

        pbxsetting::Environment projectActionEnvironment = determinationEnvironment;
        projectActionEnvironment.insertFront(context->actionSettings(), false);
        for (pbxsetting::Level const &level : context->overrideLevels()) {
            projectActionEnvironment.insertFront(level, false);
        }

        projectConfigurationFile = LoadConfigurationFile(projectConfiguration, projectActionEnvironment);
        if (projectConfigurationFile != nullptr) {
            determinationEnvironment.insertFront(projectConfigurationFile->level(), false);
        }

        targetConfiguration = ConfigurationNamed(target->buildConfigurationList(), context->configuration());
        if (targetConfiguration == nullptr) {
            fprintf(stderr, "error: unable to find target configuration %s\n", context->configuration().c_str());
            return nullptr;
        }

        determinationEnvironment.insertFront(target->settings(), false);
        determinationEnvironment.insertFront(targetConfiguration->buildSettings(), false);

        // FIXME(grp): Similar issue for the target xcconfig. These levels aren't complete (no platform) but are needed to *get* which SDK to use.
        pbxsetting::Environment targetActionEnvironment = determinationEnvironment;
        targetActionEnvironment.insertFront(context->actionSettings(), false);
        for (pbxsetting::Level const &level : context->overrideLevels()) {
            targetActionEnvironment.insertFront(level, false);
        }

        targetConfigurationFile = LoadConfigurationFile(targetConfiguration, targetActionEnvironment);
        if (targetConfigurationFile != nullptr) {
            determinationEnvironment.insertFront(targetConfigurationFile->level(), false);
        }

        determinationEnvironment.insertFront(context->actionSettings(), false);
        for (pbxsetting::Level const &level : context->overrideLevels()) {
            determinationEnvironment.insertFront(level, false);
        }

        std::string sdkroot = determinationEnvironment.resolve("SDKROOT");
        sdk = buildEnvironment.sdkManager()->findTarget(sdkroot);
        if (sdk == nullptr) {
            fprintf(stderr, "error: unable to find sdkroot %s\n", sdkroot.c_str());
            return nullptr;
        }

        buildEnvironment.specManager()->registerDomain({ sdk->platform()->name(), sdk->platform()->path() + "/Developer/Library/Xcode/Specifications" });
        specDomains = SDKSpecificationDomains(sdk);
    }

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = TargetBuildSystem(buildEnvironment.specManager(), specDomains, target);
    if (buildSystem == nullptr) {
        fprintf(stderr, "error: unable to create build system\n");
        return nullptr;
    }

    pbxspec::PBX::ProductType::shared_ptr productType = nullptr;
    pbxspec::PBX::PackageType::shared_ptr packageType = nullptr;
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        pbxproj::PBX::NativeTarget::shared_ptr nativeTarget = std::static_pointer_cast<pbxproj::PBX::NativeTarget>(target);

        productType = buildEnvironment.specManager()->productType(nativeTarget->productType(), specDomains);
        if (productType == nullptr) {
            fprintf(stderr, "error: unable to find product type %s\n", nativeTarget->productType().c_str());
            return nullptr;
        }

        // FIXME(grp): Should this always use the first package type?
        packageType = buildEnvironment.specManager()->packageType(productType->packageTypes().at(0), specDomains);
        if (packageType == nullptr) {
            fprintf(stderr, "error: unable to find package type %s\n", productType->packageTypes().at(0).c_str());
            return nullptr;
        }
    }

    // Now we have $(SDKROOT), and can make the real levels.
    pbxsetting::Environment environment = buildEnvironment.baseEnvironment();
    environment.insertFront(buildSystem->defaultSettings(), true);
    environment.insertFront(context->baseSettings(), false);
    environment.insertFront(pbxsetting::Level({
        pbxsetting::Setting::Parse("GCC_VERSION", "$(DEFAULT_COMPILER)"),
    }), false);

    environment.insertFront(sdk->platform()->defaultProperties(), false);
    environment.insertFront(PlatformArchitecturesLevel(buildEnvironment.specManager(), specDomains), false);
    environment.insertFront(sdk->defaultProperties(), false);
    environment.insertFront(sdk->platform()->settings(), false);
    environment.insertFront(sdk->settings(), false);
    environment.insertFront(sdk->customProperties(), false);
    environment.insertFront(sdk->platform()->overrideProperties(), false);

    environment.insertFront(target->project()->settings(), false);
    if (projectConfigurationFile != nullptr) {
        environment.insertFront(projectConfigurationFile->level(), false);
    }
    environment.insertFront(projectConfiguration->buildSettings(), false);

    if (packageType != nullptr) {
        environment.insertFront(PackageTypeLevel(packageType), true);
    }
    if (productType != nullptr) {
        environment.insertFront(ProductTypeLevel(productType), true);
    }

    environment.insertFront(target->settings(), false);
    environment.insertFront(targetConfiguration->buildSettings(), false);
    if (targetConfigurationFile != nullptr) {
        environment.insertFront(targetConfigurationFile->level(), false);
    }

    environment.insertFront(context->actionSettings(), false);
    for (pbxsetting::Level const &level : context->overrideLevels()) {
        environment.insertFront(level, false);
    }

    std::vector<std::string> architectures = ResolveArchitectures(environment);
    std::vector<std::string> variants = ResolveVariants(environment);
    environment.insertFront(ArchitecturesVariantsLevel(architectures, variants), false);

    environment.insertFront(pbxsetting::Level({
        pbxsetting::Setting::Parse("SDKROOT", sdk->path()),
    }), false);

    auto buildRules = std::make_shared <pbxbuild::TargetBuildRules> (pbxbuild::TargetBuildRules::Create(buildEnvironment.specManager(), specDomains, target));
    auto buildFileDisambiguation = BuildFileDisambiguation(target);
    std::string workingDirectory = target->project()->basePath();

    std::unique_ptr<TargetEnvironment> te = std::unique_ptr<TargetEnvironment>(new TargetEnvironment());
    te->_buildRules = buildRules;
    te->_environment = std::unique_ptr<pbxsetting::Environment>(new pbxsetting::Environment(environment));
    te->_variants = variants;
    te->_architectures = architectures;
    te->_buildSystem = buildSystem;
    te->_packageType = packageType;
    te->_productType = productType;
    te->_sdk = sdk;
    te->_specDomains = specDomains;
    te->_workingDirectory = workingDirectory;
    te->_buildFileDisambiguation = buildFileDisambiguation;
    return te;
}
