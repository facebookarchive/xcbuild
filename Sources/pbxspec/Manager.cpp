// Copyright 2013-present Facebook. All Rights Reserved.

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

    //
    // Do an inverse find so that we can find the overrides.
    //
    auto I = std::find_if(vector.rbegin(), vector.rend(), [&identifier](Specification::shared_ptr const &spec) -> bool {
        return identifier == spec->identifier();
    });

    if (I != vector.rend()) {
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
        if (compiler->synthesizeBuildRule()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(compiler->inputFileTypes(), compiler->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    for (Linker::shared_ptr const &linker : linkers(domains)) {
        if (linker->synthesizeBuildRule()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(linker->inputFileTypes(), linker->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    for (Tool::shared_ptr const &tool : tools(domains)) {
        if (tool->synthesizeBuildRule()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(tool->inputFileTypes(), tool->identifier()));
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
registerDomain(std::pair<std::string, std::string> const &domain)
{
    auto const &it = _domains.find(domain.first);
    if (it == _domains.end()) {
        Context context = {
            .manager = this,
            .domain = domain.first,
        };

        if (FSUtil::TestForDirectory(domain.second)) {
            FSUtil::EnumerateRecursive(domain.second, "*.xcspec", [&](std::string const &filename) -> bool {
                if (!FSUtil::TestForDirectory(filename)) {
#if 0
                    fprintf(stderr, "importing specification '%s'\n", filename.c_str());
#endif
                    if (!Specification::Open(&context, filename)) {
                        fprintf(stderr, "warning: failed to import specification '%s'\n", filename.c_str());
                    }
                }
                return true;
            });
        } else {
#if 0
            fprintf(stderr, "importing specification '%s'\n", domain.second.c_str());
#endif
            if (!Specification::Open(&context, domain.second)) {
                fprintf(stderr, "warning: failed to import specification '%s'\n", domain.second.c_str());
            }
        }

        _domains.insert(domain);
    }
}

void Manager::
registerBuildRules(std::string const &path)
{
    plist::Object *plist = plist::Object::Parse(path);
    if (plist == nullptr) {
        return;
    }

    if (auto array = plist::CastTo <plist::Array> (plist)) {
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

    plist->release();
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

std::vector<std::string> Manager::
AnyDomain(std::string const &preferred)
{
    return { preferred, AnyDomain() };
}

std::pair<std::string, std::string> Manager::
DefaultDomain(std::string const &developerRoot)
{
    return { "default", developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents" };
}

std::vector<std::pair<std::string, std::string>> Manager::
EmbeddedDomains(std::string const &developerRoot)
{
    std::string root = developerRoot + "/../PlugIns/IDEiOSSupportCore.ideplugin/Contents/Resources";
    return {
        { "embedded-shared", root + "/Embedded-Shared.xcspec" },
        { "embedded", root + "/Embedded-Device.xcspec" },
        { "embedded-simulator", root + "/Embedded-Simulator.xcspec" },
    };
}

std::vector<std::pair<std::string, std::string>> Manager::
PlatformDomains(std::string const &developerRoot)
{
    std::string root = developerRoot + "/..";
    return {
        { "iphoneos", root + "/Developer/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources/Device.xcspec" },
        { "iphonesimulator", root + "/Developer/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources/Simulator.xcspec" },
        { "watchos", root + "/Developer/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources/Device.xcspec" },
        { "watchsimulator", root + "/Developer/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources/Simulator.xcspec" },
        { "watchos", root + "/Developer/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources/Shared.xcspec" },
        { "watchsimulator", root + "/Developer/Platforms/iPhoneOS.platform/Developer/Library/Xcode/PrivatePlugIns/IDEiOSPlatformSupportCore.ideplugin/Contents/Resources/Shared.xcspec" },
    };
}

std::string Manager::
DeveloperBuildRules(std::string const &developerRoot)
{
    return developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents/Frameworks/DevToolsCore.framework/Versions/A/Resources/BuiltInBuildRules.plist";
}
