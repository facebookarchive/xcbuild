/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/Manager.h>
#include <pbxspec/Context.h>
#include <plist/Array.h>
#include <plist/Dictionary.h>
#include <plist/Object.h>
#include <plist/Format/Any.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

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
using pbxspec::PBX::Tool;
using libutil::Filesystem;
using libutil::FSUtil;

Manager::
Manager()
{
}

Manager::
~Manager()
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
                    for (auto const &s : it->second) {
                        specifications.emplace_back(std::static_pointer_cast<T>(s));
                    }
                }
            }
        } else {
            auto const &doit = _specifications.find(domain);
            if (doit != _specifications.end()) {
                auto const &it = doit->second.find(type);
                if (it != doit->second.end()) {
                    for (auto const &s : it->second) {
                        specifications.emplace_back(std::static_pointer_cast<T>(s));
                    }
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

bool Manager::
inheritSpecification(PBX::Specification::shared_ptr const &specification)
{
    if (specification->basedOnIdentifier() && specification->basedOnDomain() && specification->base() == nullptr) {
        /*
         * Search the specified domain then in any domain. Some specifications inherit
         * from domain/identifier pairs that don't exist in practice, but by using any
         * domain they can successfully inherit from something potentially relevant.
         */
        std::vector<std::string> domains = { *specification->basedOnDomain(), AnyDomain() };

        /* Find the base specification. */
        auto base = this->specification(specification->type(), *specification->basedOnIdentifier(), domains);
        if (base == nullptr) {
            fprintf(stderr, "error: cannot find base %s specification '%s:%s'\n", specification->type(), specification->basedOnDomain()->c_str(), specification->basedOnIdentifier()->c_str());
            return false;
        }

#if 0
        fprintf(stderr, "debug: inheriting %s:%s (%s) from %s:%s (%s)\n", specification->domain().c_str(), specification->identifier().c_str(), specification->type(), base->domain().c_str(), base->identifier().c_str(), base->type());
#endif

        /*
         * Ensure the base specification itself has been inherited. Since inheritance
         * copies values from the base, the base must have its own values set first.
         */
        if (!inheritSpecification(base)) {
            return false;
        }

        /* Perform inheritance. */
        if (!specification->inherit(base)) {
            fprintf(stderr, "error: could not inherit from base %s specification '%s:%s'\n", specification->type(), specification->basedOnDomain()->c_str(), specification->basedOnIdentifier()->c_str());
            return false;
        }
    }

    return true;
}

void Manager::
registerDomains(Filesystem const *filesystem, std::vector<std::pair<std::string, std::string>> const &domains)
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

        if (filesystem->isDirectory(domain.second)) {
            filesystem->enumerateRecursive(domain.second, [&](std::string const &filename) -> bool {
                /* Support both *.xcspec and *.pbfilespec as a few of the latter remain in use. */
                if (FSUtil::GetFileExtension(filename) != "xcspec" && FSUtil::GetFileExtension(filename) != "pbfilespec") {
                    return true;
                }

                /* For *.pbfilespec files, default to FileType specifications. */
                bool file = FSUtil::GetFileExtension(filename) == "pbfilespec";
                context.defaultType = (file ? "FileType" : std::string());

                if (!filesystem->isDirectory(filename)) {
#if 0
                    fprintf(stderr, "importing specification '%s'\n", filename.c_str());
#endif

                    ext::optional<PBX::Specification::vector> fileSpecifications = Specification::Open(filesystem, &context, filename);
                    if (fileSpecifications) {
                        specifications.insert(specifications.end(), fileSpecifications->begin(), fileSpecifications->end());
                    } else {
                        fprintf(stderr, "warning: failed to import specification '%s'\n", filename.c_str());
                    }
                }
                return true;
            });
        } else if (filesystem->exists(domain.second)) {
#if 0
            fprintf(stderr, "importing specification '%s'\n", domain.second.c_str());
#endif
            ext::optional<PBX::Specification::vector> fileSpecifications = Specification::Open(filesystem, &context, domain.second);
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
     * Inherit from existing and newly added specifications.
     */
    for (PBX::Specification::shared_ptr const &specification : specifications) {
        if (!inheritSpecification(specification)) {
            /* Unfortunately, not much useful error handling to do here. */
            continue;
        }
    }
}

bool Manager::
registerBuildRules(Filesystem const *filesystem, std::string const &path)
{
    std::vector<uint8_t> contents;
    if (!filesystem->read(&contents, path)) {
        return false;
    }

    std::unique_ptr<plist::Object> plist = plist::Format::Any::Deserialize(contents).first;
    if (plist == nullptr) {
        return false;
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

    return true;
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
    std::string embedded   = developerRoot + "/../PlugIns/IDEiOSSupportCore.ideplugin/Contents/Resources";

    return {
        { "default", developerRoot + "/Library/Xcode/Specifications" },
        { "default", frameworks + "/" + "DevToolsCore.framework" },
        { "default", plugins + "/" + "Clang LLVM 1.0.xcplugin" },
        { "default", plugins + "/" + "Core Data.xcplugin" },
        { "default", plugins + "/" + "CoreBuildTasks.xcplugin" },
        { "default", plugins + "/" + "IBCompilerPlugin.xcplugin" },
        { "default", plugins + "/" + "Metal.xcplugin" },
        { "default", plugins + "/" + "SceneKit.xcplugin" },
        { "default", plugins + "/" + "SpriteKit.xcplugin" },
        { "default", plugins + "/" + "XCLanguageSupport.xcplugin" },
        { "embedded-shared", embedded + "/" + "Embedded-Shared.xcspec" },
        { "embedded", embedded + "/" + "Embedded-Device.xcspec" },
        { "embedded-simulator", embedded + "/" + "Embedded-Simulator.xcspec" },
    };
}

std::vector<std::pair<std::string, std::string>> Manager::
PlatformDomains(std::unordered_map<std::string, std::string> const &platforms)
{
    std::vector<std::pair<std::string, std::string>> domains;

    auto iphoneos = platforms.find("iphoneos");
    if (iphoneos != platforms.end()) {
        std::string root = iphoneos->second + "/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources";
        domains.push_back({ "iphoneos-shared", root + "/" + "Shared.xcspec" });
        domains.push_back({ "iphoneos", root + "/" + "Device.xcspec" });
        domains.push_back({ "iphonesimulator", root + "/" + "Simulator.xcspec" });
    }

    auto appletvos = platforms.find("appletvos");
    if (appletvos != platforms.end()) {
        std::string root = appletvos->second + "/Developer/Library/Xcode/PrivatePlugIns/IDEAppleTVSupportCore.ideplugin/Contents/Resources";
        domains.push_back({ "appletvos-shared", root + "/" + "Shared.xcspec" });
        domains.push_back({ "appletvos", root + "/" + "Device.xcspec" });
        domains.push_back({ "appletvsimulator", root + "/" + "Simulator.xcspec" });
    }

    auto watchos = platforms.find("watchos");
    if (watchos != platforms.end()) {
        std::string root = watchos->second + "/Developer/Library/Xcode/PrivatePlugIns/IDEWatchSupportCore.ideplugin/Contents/Resources";
        domains.push_back({ "watchos-shared", root + "/" + "Shared.xcspec" });
        domains.push_back({ "watchos", root + "/" + "Device.xcspec" });
        domains.push_back({ "watchsimulator", root + "/" + "Simulator.xcspec" });
    }

    /* The standard platform specifications directory. */
    for (auto const &platform : platforms) {
        domains.push_back({ platform.first, platform.second + "/Developer/Library/Xcode/Specifications" });
    }

    return domains;
}

std::vector<std::pair<std::string, std::string>> Manager::
PlatformDependentDomains(std::string const &developerRoot)
{
    std::vector<std::pair<std::string, std::string>> domains;

    std::string path = developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins/XCWatchKit1Support.xcplugin";
    domains.push_back({ "default", path });

    return domains;
}

std::vector<std::string> Manager::
DeveloperBuildRules(std::string const &developerRoot)
{
    return {
        developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents/Frameworks/DevToolsCore.framework/Resources/BuiltInBuildRules.plist",
        developerRoot + "/Library/Xcode/Specifications/BuiltInBuildRules.plist",
    };
}

