/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/Manager.h>
#include <pbxspec/Context.h>

using pbxspec::Manager;
using pbxspec::Context;
using pbxspec::PBX::Specification;
using pbxspec::PBX::Architecture;
using pbxspec::PBX::BuildPhase;
using pbxspec::PBX::BuildRule;
using pbxspec::PBX::BuildSettings;
using pbxspec::PBX::BuildStep;
using pbxspec::PBX::BuildSystem;
using pbxspec::PBX::Compiler;
using pbxspec::PBX::FileType;
using pbxspec::PBX::Linker;
using pbxspec::PBX::PackageType;
using pbxspec::PBX::ProductType;
using pbxspec::PBX::PropertyConditionFlavor;
using pbxspec::PBX::Tool;
using libutil::FSUtil;

Manager::Manager()
{
}

Manager::~Manager()
{
}

template <typename T>
typename T::vector Manager::
findSpecifications(std::vector<std::string> const &domains, char const *type) const
{
    if (type == nullptr) {
        return typename T::vector();
    }

    typename T::vector specifications;

    for (std::string const &domain : domains) {
        if (domain == AnyDomain()) {
            for (auto const &entry : _specifications) {
                auto const &it = entry.second.find(type);
                if (it != entry.second.end()) {
                    typename T::vector domainSpecifications = reinterpret_cast <typename T::vector const &> (it->second);
                    specifications.insert(specifications.end(), domainSpecifications.begin(), domainSpecifications.end());
                }
            }
        } else {
            auto const &doit = _specifications.find(domain);
            if (doit != _specifications.end()) {
                auto const &it = doit->second.find(type);
                if (it != doit->second.end()) {
                    typename T::vector domainSpecifications = reinterpret_cast <typename T::vector const &> (it->second);
                    specifications.insert(specifications.end(), domainSpecifications.begin(), domainSpecifications.end());
                }
            }
        }
    }

    return specifications;
}

template <typename T>
typename T::shared_ptr Manager::
findSpecification(std::vector<std::string> const &domains, std::string const &identifier, char const *type) const
{
    typename T::vector vector = findSpecifications <T> (domains, type);

    auto I = std::find_if(vector.begin(), vector.end(), [&identifier](Specification::shared_ptr const &spec) -> bool {
        return identifier == spec->identifier();
    });

    if (I != vector.end()) {
        return *I;
    }

    return nullptr;
}

Specification::shared_ptr Manager::
specification(char const *type, std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <Specification> (domains, identifier, type);
}

Specification::vector Manager::
specifications(char const *type, std::vector<std::string> const &domains) const
{
    return findSpecifications <Specification> (domains, type);
}

Architecture::shared_ptr Manager::
architecture(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <Architecture> (domains, identifier);
}

Architecture::vector Manager::
architectures(std::vector<std::string> const &domains) const
{
    return findSpecifications <Architecture> (domains);
}

BuildPhase::shared_ptr Manager::
buildPhase(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <BuildPhase> (domains, identifier);
}

BuildPhase::vector Manager::
buildPhases(std::vector<std::string> const &domains) const
{
    return findSpecifications <BuildPhase> (domains);
}

BuildSettings::shared_ptr Manager::
buildSettings(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <BuildSettings> (domains, identifier);
}

BuildSettings::vector Manager::
buildSettingses(std::vector<std::string> const &domains) const
{
    return findSpecifications <BuildSettings> (domains);
}

BuildStep::shared_ptr Manager::
buildStep(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <BuildStep> (domains, identifier);
}

BuildStep::vector Manager::
buildSteps(std::vector<std::string> const &domains) const
{
    return findSpecifications <BuildStep> (domains);
}

BuildSystem::shared_ptr Manager::
buildSystem(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <BuildSystem> (domains, identifier);
}

BuildSystem::vector Manager::
buildSystems(std::vector<std::string> const &domains) const
{
    return findSpecifications <BuildSystem> (domains);
}

Compiler::shared_ptr Manager::
compiler(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <Compiler> (domains, identifier);
}

Compiler::vector Manager::
compilers(std::vector<std::string> const &domains) const
{
    return findSpecifications <Compiler> (domains);
}

FileType::shared_ptr Manager::
fileType(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <FileType> (domains, identifier);
}

FileType::vector Manager::
fileTypes(std::vector<std::string> const &domains) const
{
    return findSpecifications <FileType> (domains);
}

Linker::shared_ptr Manager::
linker(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <Linker> (domains, identifier);
}

Linker::vector Manager::
linkers(std::vector<std::string> const &domains) const
{
    return findSpecifications <Linker> (domains);
}

PackageType::shared_ptr Manager::
packageType(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <PackageType> (domains, identifier);
}

PackageType::vector Manager::
packageTypes(std::vector<std::string> const &domains) const
{
    return findSpecifications <PackageType> (domains);
}

ProductType::shared_ptr Manager::
productType(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <ProductType> (domains, identifier);
}

ProductType::vector Manager::
productTypes(std::vector<std::string> const &domains) const
{
    return findSpecifications <ProductType> (domains);
}

PropertyConditionFlavor::shared_ptr Manager::
propertyConditionFlavor(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <PropertyConditionFlavor> (domains, identifier);
}

PropertyConditionFlavor::vector Manager::
propertyConditionFlavors(std::vector<std::string> const &domains) const
{
    return findSpecifications <PropertyConditionFlavor> (domains);
}

Tool::shared_ptr Manager::
tool(std::string const &identifier, std::vector<std::string> const &domains) const
{
    return findSpecification <Tool> (domains, identifier);
}

Tool::vector Manager::
tools(std::vector<std::string> const &domains) const
{
    return findSpecifications <Tool> (domains);
}

BuildRule::vector Manager::
synthesizedBuildRules(std::vector<std::string> const &domains) const
{
    BuildRule::vector buildRules;

    for (Compiler::shared_ptr const &compiler : compilers(domains)) {
        if (compiler->synthesizeBuildRule() && compiler->inputFileTypes()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(*compiler->inputFileTypes(), compiler->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    for (Linker::shared_ptr const &linker : linkers(domains)) {
        if (linker->synthesizeBuildRule() && linker->inputFileTypes()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(*linker->inputFileTypes(), linker->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    for (Tool::shared_ptr const &tool : tools(domains)) {
        if (tool->synthesizeBuildRule() && tool->inputFileTypes()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(*tool->inputFileTypes(), tool->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    return buildRules;
}

void Manager::
addSpecification(PBX::Specification::shared_ptr const &spec)
{
    if (!spec) {
        fprintf(stderr, "error: registering null specification\n");
        return;
    }

    if (spec->type() == nullptr) {
        fprintf(stderr, "error: registering a specification with null type\n");
        return;
    }

    if (auto ospec = specification(spec->type(), spec->identifier(), { spec->domain() })) {
        /*
         * Workaround for a typo in the default specifications; don't warn.
         */
        if (spec->identifier() == "PlatformStandardUniversal") {
            return;
        }

        fprintf(stderr, "error: registering %s specification '%s' in domain %s twice\n",
                spec->type(), spec->identifier().c_str(), spec->domain().c_str());
        return;
    }

#if 0
    fprintf(stderr, "adding %s spec to domain %s '%s'\n",
            spec->type(), spec->domain().c_str(), spec->identifier().c_str());
#endif
    _specifications[spec->domain()][spec->type()].push_back(spec);
}

void Manager::
registerDomains(std::vector<std::pair<std::string, std::string>> const &domains)
{
    PBX::Specification::vector specifications;

    for (auto const &domain : domains) {
        /*
         * Avoid double domain registration. Unncessary and causes warnings.
         */
        if (_domains.find(domain.first) != _domains.end()) {
            continue;
        }

        Context context = {
            .domain = domain.first,
        };

        if (FSUtil::TestForDirectory(domain.second)) {
            FSUtil::EnumerateRecursive(domain.second, [&](std::string const &filename) -> bool {
                /* Support both *.xcspec and *.pbfilespec as a few of the latter remain in use. */
                if (FSUtil::GetFileExtension(filename) != "xcspec" && FSUtil::GetFileExtension(filename) != "pbfilespec") {
                    return true;
                }

                /* For *.pbfilespec files, default to FileType specifications. */
                bool file = FSUtil::GetFileExtension(filename) == "pbfilespec";
                context.defaultType = (file ? "FileType" : std::string());

                if (!FSUtil::TestForDirectory(filename)) {
#if 0
                    fprintf(stderr, "importing specification '%s'\n", filename.c_str());
#endif

                    ext::optional<PBX::Specification::vector> fileSpecifications = Specification::Open(&context, filename);
                    if (fileSpecifications) {
                        specifications.insert(specifications.end(), fileSpecifications->begin(), fileSpecifications->end());
                    } else {
                        fprintf(stderr, "warning: failed to import specification '%s'\n", filename.c_str());
                    }
                }
                return true;
            });
        } else {
#if 0
            fprintf(stderr, "importing specification '%s'\n", domain.second.c_str());
#endif
            ext::optional<PBX::Specification::vector> fileSpecifications = Specification::Open(&context, domain.second);
            if (fileSpecifications) {
                specifications.insert(specifications.end(), fileSpecifications->begin(), fileSpecifications->end());
            } else {
                fprintf(stderr, "warning: failed to import specification '%s'\n", domain.second.c_str());
            }
        }
    }

    /*
     * Mark all of the domains regsitered. This is after all of the inputs so the
     * same domain can be registered multiple times (loaded from multiple paths)
     * in a single registration, but can't be registered twice outside that.
     */
    for (auto const &domain : domains) {
        _domains.insert(domain.first);
    }

    /*
     * Register all specifications. Must be before inheritance in case specifications
     * inherit from other specifications also being registered at the same time.
     */
    for (PBX::Specification::shared_ptr const &specification : specifications) {
        addSpecification(specification);
    }

    /*
     * Inherit from existing specifications.
     */
    for (PBX::Specification::shared_ptr const &specification : specifications) {
        if (specification->basedOnIdentifier() && specification->basedOnDomain()) {
            /*
             * Search the specified domain then the base domain. Some specifications inherit
             * from domain/identifier pairs that don't exist in practice, but by using the
             * default domain they can successfully inherit.
             */
            std::vector<std::string> domains = { *specification->basedOnDomain(), "default" };

            /* Find the base specification. */
            auto base = this->specification(specification->type(), *specification->basedOnIdentifier(), domains);
            if (base == nullptr) {
                fprintf(stderr, "error: cannot find base %s specification '%s:%s'\n", specification->type(), specification->basedOnDomain()->c_str(), specification->basedOnIdentifier()->c_str());
                continue;
            }

            /* Perform inheritance. */
            if (!specification->inherit(base)) {
                fprintf(stderr, "error: could not inherit from base %s specification '%s:%s'\n", specification->type(), specification->basedOnDomain()->c_str(), specification->basedOnIdentifier()->c_str());
                continue;
            }
        }
    }
}

void Manager::
registerBuildRules(std::string const &path)
{
    std::unique_ptr<plist::Object> plist = plist::Format::Any::Read(path).first;
    if (plist == nullptr) {
        return;
    }

    if (auto array = plist::CastTo <plist::Array> (plist.get())) {
        size_t count  = array->count();
        for (size_t n = 0; n < count; n++) {
            if (auto dict = array->value <plist::Dictionary> (n)) {
                BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule());
                if (buildRule->parse(dict)) {
                    _buildRules.push_back(buildRule);
                }
            }
        }
    }
}

Manager::shared_ptr Manager::
Create(void)
{
    return std::make_shared <Manager> ();
}

std::string Manager::
AnyDomain()
{
    return "<<domain>>";
}

std::vector<std::pair<std::string, std::string>> Manager::
DefaultDomains(std::string const &developerRoot)
{
    std::string root       = developerRoot + "/../PlugIns/Xcode3Core.ideplugin";
    std::string frameworks = root + "/Contents/Frameworks";
    std::string plugins    = root + "/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins";

    return {
        { "default", frameworks + "/" + "DevToolsCore.framework" },
        { "default", plugins + "/" + "Clang LLVM 1.0.xcplugin" },
        { "default", plugins + "/" + "Core Data.xcplugin" },
        { "default", plugins + "/" + "CoreBuildTasks.xcplugin" },
        { "default", plugins + "/" + "IBCompilerPlugin.xcplugin" },
        { "default", plugins + "/" + "Metal.xcplugin" },
        { "default", plugins + "/" + "SceneKit.xcplugin" },
        { "default", plugins + "/" + "SpriteKit.xcplugin" },
        { "default", plugins + "/" + "XCLanguageSupport.xcplugin" },
    };
}

std::vector<std::pair<std::string, std::string>> Manager::
EmbeddedDomains(std::string const &developerRoot)
{
    std::string root = developerRoot + "/../PlugIns/IDEiOSSupportCore.ideplugin/Contents/Resources";
    return {
        { "embedded-shared", root + "/" + "Embedded-Shared.xcspec" },
        { "embedded", root + "/" + "Embedded-Device.xcspec" },
        { "embedded-simulator", root + "/" + "Embedded-Simulator.xcspec" },
    };
}

std::vector<std::pair<std::string, std::string>> Manager::
PlatformDomains(std::string const &developerRoot, std::string const &platformName, std::string const &platformPath)
{
    std::vector<std::pair<std::string, std::string>> domains;

    std::string root = developerRoot + "/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources";
    if (platformName == "iphoneos" || platformName == "watchos" || platformName == "appletvos") {
        domains.push_back({ platformName, root + "/" + "Device.xcspec" });
    } else if (platformName == "iphonesimulator" || platformName == "watchsimulator" || platformName == "appletvsimulator") {
        domains.push_back({ platformName, root + "/" + "Simulator.xcspec" });
    } else {
        /* The standard platform specifications directory. */
        domains.push_back({ platformName, platformPath + "/Developer/Library/Xcode/Specifications" });
    }

    if (platformName == "iphoneos" || platformName == "iphonesimulator") {
        std::string path = developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins/XCWatchKit1Support.xcplugin";
        domains.push_back({ platformName, path });
    }

    return domains;
}

std::string Manager::
DeveloperBuildRules(std::string const &developerRoot)
{
    std::string root = developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents/Frameworks/DevToolsCore.framework/Resources";
    return root + "/" + "BuiltInBuildRules.plist";
}

