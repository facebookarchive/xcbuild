/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Target/Environment.h>
#include <pbxbuild/Build/Context.h>

namespace Build = pbxbuild::Build;
namespace Target = pbxbuild::Target;
using libutil::FSUtil;

Target::Environment::
Environment()
{
}

static std::unordered_map<pbxproj::PBX::BuildFile::shared_ptr, std::string>
BuildFileDisambiguation(pbxproj::PBX::Target::shared_ptr const &target)
{
    std::unordered_multimap<std::string, pbxproj::PBX::BuildFile::shared_ptr> buildFileUnambiguous;
    std::unordered_map<pbxproj::PBX::BuildFile::shared_ptr, std::string> buildFileDisambiguation;

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        if (buildPhase->type() != pbxproj::PBX::BuildPhase::kTypeSources) {
            continue;
        }

        for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
            std::string name = FSUtil::GetBaseNameWithoutExtension(buildFile->fileRef()->name());

            /* Use a case-insensitive key to detect conflicts. */
            std::string lower;
            std::transform(name.begin(), name.end(), std::back_inserter(lower), ::tolower);

            auto range = buildFileUnambiguous.equal_range(lower);
            if (range.first != buildFileUnambiguous.end()) {
                /* Conflicts with at least one other file, add a disambiguation. */
                buildFileDisambiguation.insert({ buildFile, name + "-" + buildFile->blueprintIdentifier() });

                /* Add disambiguations for all the conflicting files. */
                for (auto it = range.first; it != range.second; ++it) {
                    pbxproj::PBX::BuildFile::shared_ptr const &otherBuildFile = it->second;
                    std::string otherName = FSUtil::GetBaseNameWithoutExtension(otherBuildFile->fileRef()->name());
                    buildFileDisambiguation.insert({ otherBuildFile, otherName + "-" + otherBuildFile->blueprintIdentifier() });
                }
            }
            buildFileUnambiguous.insert({ lower, buildFile });
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
        ext::optional<pbxsetting::Setting> architectureSetting = architecture->defaultSetting();
        if (architectureSetting) {
            architectureSettings.push_back(*architectureSetting);
        }
        if (!architecture->realArchitectures()) {
            if (std::find(platformArchitectures.begin(), platformArchitectures.end(), architecture->identifier()) == platformArchitectures.end()) {
                platformArchitectures.push_back(architecture->identifier());
            }
        }
    }

    architectureSettings.push_back(pbxsetting::Setting::Create("VALID_ARCHS", pbxsetting::Type::FormatList(platformArchitectures)));

    return pbxsetting::Level(architectureSettings);
}

static pbxsetting::Level
PackageTypeLevel(pbxspec::PBX::PackageType::shared_ptr const &packageType)
{
    std::vector<pbxsetting::Setting> settings = {
        pbxsetting::Setting::Create("PACKAGE_TYPE", packageType->identifier()),
    };

    if (packageType->defaultBuildSettings()) {
        pbxsetting::Level const &packageTypeLevel = *packageType->defaultBuildSettings();
        settings.insert(settings.end(), packageTypeLevel.settings().begin(), packageTypeLevel.settings().end());
    }

    return pbxsetting::Level(settings);
}


static pbxsetting::Level
ProductTypeLevel(pbxspec::PBX::ProductType::shared_ptr const &productType)
{
    std::vector<pbxsetting::Setting> settings = {
        pbxsetting::Setting::Create("PRODUCT_TYPE", productType->identifier()),
    };

    if (productType->defaultBuildProperties()) {
        pbxsetting::Level const &productTypeLevel = *productType->defaultBuildProperties();
        settings.insert(settings.end(), productTypeLevel.settings().begin(), productTypeLevel.settings().end());
    }

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
        settings.push_back(pbxsetting::Setting::Create("CURRENT_VARIANT", variants.front()));
        settings.push_back(pbxsetting::Setting::Create("variant", variants.front()));
    }

    if (!architectures.empty()) {
        settings.push_back(pbxsetting::Setting::Create("CURRENT_ARCH", architectures.front()));
        settings.push_back(pbxsetting::Setting::Create("arch", architectures.front()));
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

ext::optional<Target::Environment> Target::Environment::
Create(Build::Environment const &buildEnvironment, Build::Context const &buildContext, pbxproj::PBX::Target::shared_ptr const &target)
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
        determinationEnvironment.insertFront(buildContext.baseSettings(), false);

        projectConfiguration = ConfigurationNamed(target->project()->buildConfigurationList(), buildContext.configuration());
        if (projectConfiguration == nullptr) {
            fprintf(stderr, "error: unable to find project configuration %s\n", buildContext.configuration().c_str());
            return ext::nullopt;
        }

        determinationEnvironment.insertFront(target->project()->settings(), false);
        determinationEnvironment.insertFront(projectConfiguration->buildSettings(), false);

        pbxsetting::Environment projectActionEnvironment = determinationEnvironment;
        projectActionEnvironment.insertFront(buildContext.actionSettings(), false);
        for (pbxsetting::Level const &level : buildContext.overrideLevels()) {
            projectActionEnvironment.insertFront(level, false);
        }

        projectConfigurationFile = LoadConfigurationFile(projectConfiguration, projectActionEnvironment);
        if (projectConfigurationFile != nullptr) {
            determinationEnvironment.insertFront(projectConfigurationFile->level(), false);
        }

        targetConfiguration = ConfigurationNamed(target->buildConfigurationList(), buildContext.configuration());
        if (targetConfiguration == nullptr) {
            fprintf(stderr, "error: unable to find target configuration %s\n", buildContext.configuration().c_str());
            return ext::nullopt;
        }

        determinationEnvironment.insertFront(target->settings(), false);
        determinationEnvironment.insertFront(targetConfiguration->buildSettings(), false);

        // FIXME(grp): Similar issue for the target xcconfig. These levels aren't complete (no platform) but are needed to *get* which SDK to use.
        pbxsetting::Environment targetActionEnvironment = determinationEnvironment;
        targetActionEnvironment.insertFront(buildContext.actionSettings(), false);
        for (pbxsetting::Level const &level : buildContext.overrideLevels()) {
            targetActionEnvironment.insertFront(level, false);
        }

        targetConfigurationFile = LoadConfigurationFile(targetConfiguration, targetActionEnvironment);
        if (targetConfigurationFile != nullptr) {
            determinationEnvironment.insertFront(targetConfigurationFile->level(), false);
        }

        determinationEnvironment.insertFront(buildContext.actionSettings(), false);
        for (pbxsetting::Level const &level : buildContext.overrideLevels()) {
            determinationEnvironment.insertFront(level, false);
        }

        std::string sdkroot = determinationEnvironment.resolve("SDKROOT");
        sdk = buildEnvironment.sdkManager()->findTarget(sdkroot);
        if (sdk == nullptr) {
            fprintf(stderr, "error: unable to find sdkroot %s\n", sdkroot.c_str());
            return ext::nullopt;
        }

        buildEnvironment.specManager()->registerDomains({
            pbxspec::Manager::PlatformDomain(
                buildEnvironment.sdkManager()->path(),
                sdk->platform()->name(),
                sdk->platform()->path()),
        });
        specDomains = SDKSpecificationDomains(sdk);
    }

    pbxspec::PBX::BuildSystem::shared_ptr buildSystem = TargetBuildSystem(buildEnvironment.specManager(), specDomains, target);
    if (buildSystem == nullptr) {
        fprintf(stderr, "error: unable to create build system\n");
        return ext::nullopt;
    }

    pbxspec::PBX::ProductType::shared_ptr productType = nullptr;
    pbxspec::PBX::PackageType::shared_ptr packageType = nullptr;
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        pbxproj::PBX::NativeTarget::shared_ptr nativeTarget = std::static_pointer_cast<pbxproj::PBX::NativeTarget>(target);

        productType = buildEnvironment.specManager()->productType(nativeTarget->productType(), specDomains);
        if (productType == nullptr) {
            fprintf(stderr, "error: unable to find product type %s\n", nativeTarget->productType().c_str());
            return ext::nullopt;
        }

        // FIXME(grp): Should this always use the first package type?
        if (productType->packageTypes() && !productType->packageTypes()->empty()) {
            packageType = buildEnvironment.specManager()->packageType(productType->packageTypes()->at(0), specDomains);
            if (packageType == nullptr) {
                fprintf(stderr, "error: unable to find package type %s\n", productType->packageTypes()->at(0).c_str());
                return ext::nullopt;
            }
        }
    }

    // Now we have $(SDKROOT), and can make the real levels.
    pbxsetting::Environment environment = buildEnvironment.baseEnvironment();
    environment.insertFront(buildSystem->defaultSettings(), true);
    environment.insertFront(buildContext.baseSettings(), false);
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

    if (packageType != nullptr) {
        environment.insertFront(PackageTypeLevel(packageType), false);
    }
    if (productType != nullptr) {
        environment.insertFront(ProductTypeLevel(productType), false);
    }

    environment.insertFront(target->project()->settings(), false);
    if (projectConfigurationFile != nullptr) {
        environment.insertFront(projectConfigurationFile->level(), false);
    }
    environment.insertFront(projectConfiguration->buildSettings(), false);

    environment.insertFront(target->settings(), false);
    if (targetConfigurationFile != nullptr) {
        environment.insertFront(targetConfigurationFile->level(), false);
    }
    environment.insertFront(targetConfiguration->buildSettings(), false);

    environment.insertFront(buildContext.actionSettings(), false);
    for (pbxsetting::Level const &level : buildContext.overrideLevels()) {
        environment.insertFront(level, false);
    }

    std::vector<std::string> architectures = ResolveArchitectures(environment);
    std::vector<std::string> variants = ResolveVariants(environment);
    environment.insertFront(ArchitecturesVariantsLevel(architectures, variants), false);

    /* At the target level and below, the SDKROOT changes to always be a SDK path. */
    environment.insertFront(pbxsetting::Level({
        pbxsetting::Setting::Create("SDKROOT", sdk->path()),
    }), false);

    /* Determine toolchains. Must be after the SDK levels are added, so they can be a fallback. */
    xcsdk::SDK::Toolchain::vector toolchains;
    for (std::string const &toolchainName : pbxsetting::Type::ParseList(environment.resolve("TOOLCHAINS"))) {
        if (xcsdk::SDK::Toolchain::shared_ptr toolchain = buildEnvironment.sdkManager()->findToolchain(toolchainName)) {
            toolchains.push_back(toolchain);
        }
    }

    /* Tool search directories. Use the toolchains just discovered. */
    std::vector<std::string> executablePaths = sdk->executablePaths(toolchains);

    auto buildRules = std::make_shared<Target::BuildRules>(Target::BuildRules::Create(buildEnvironment.specManager(), specDomains, target));
    auto buildFileDisambiguation = BuildFileDisambiguation(target);
    std::string workingDirectory = target->project()->basePath();

    Target::Environment te = Target::Environment();
    te._sdk = sdk;
    te._toolchains = toolchains;
    te._executablePaths = executablePaths;
    te._buildRules = buildRules;
    te._environment = std::unique_ptr<pbxsetting::Environment>(new pbxsetting::Environment(environment));
    te._variants = variants;
    te._architectures = architectures;
    te._buildSystem = buildSystem;
    te._packageType = packageType;
    te._productType = productType;
    te._specDomains = specDomains;
    te._workingDirectory = workingDirectory;
    te._buildFileDisambiguation = buildFileDisambiguation;
    return te;
}
