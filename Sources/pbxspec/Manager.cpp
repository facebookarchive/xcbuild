// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/Manager.h>
#include <pbxspec/Context.h>

using pbxspec::Manager;
using pbxspec::Context;
using pbxspec::PBX::Specification;
using pbxspec::PBX::Architecture;
using pbxspec::PBX::BuildPhase;
using pbxspec::PBX::BuildRule;
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
findSpecifications(std::string const &domain, char const *type) const
{
    if (type == nullptr) {
        return typename T::vector();
    }

    auto const &doit = _specifications.find(domain);
    if (doit != _specifications.end()) {
        auto const &it = doit->second.find(type);
        if (it != doit->second.end()) {
            typename T::vector specifications = reinterpret_cast <typename T::vector const &> (it->second);
            return specifications;
        }
    }

    return typename T::vector();
}

template <typename T>
typename T::shared_ptr Manager::
findSpecification(std::string const &domain, std::string const &identifier, char const *type, bool onlyDefault) const
{
    typename T::vector vector = findSpecifications <T> (domain, type);

    //
    // Do an inverse find so that we can find the overrides.
    //
    auto I = std::find_if(
            vector.rbegin(), vector.rend(),
            [&identifier, onlyDefault](Specification::shared_ptr const &spec) -> bool
            {
                return ((!onlyDefault || spec->isDefault()) &&
                        identifier == spec->identifier());
            });

    if (I == vector.rend()) {
        //
        // We couldn't find what the user wants, use any identifier found.
        //
        I = std::find_if(vector.rbegin(), vector.rend(),
                [&identifier](Specification::shared_ptr const &spec) -> bool
                {
                    return identifier == spec->identifier();
                });

        if (I == vector.rend()) {
            if (domain != GlobalDomain()) {
                return findSpecification<T>(GlobalDomain(), identifier, type, onlyDefault);
            } else {
                return nullptr;
            }
        }
    }

    return *I;
}

Specification::shared_ptr Manager::
specification(char const *type, std::string const &identifier, std::string const &domain, bool onlyDefault) const
{
    return findSpecification <Specification> (domain, identifier, type, onlyDefault);
}

Specification::vector Manager::
specifications(char const *type, std::string const &domain) const
{
    return findSpecifications <Specification> (domain, type);
}

Architecture::shared_ptr Manager::
architecture(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <Architecture> (domain, identifier);
}

Architecture::vector Manager::
architectures(std::string const &domain) const
{
    return findSpecifications <Architecture> (domain);
}

BuildPhase::shared_ptr Manager::
buildPhase(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <BuildPhase> (domain, identifier);
}

BuildPhase::vector Manager::
buildPhases(std::string const &domain) const
{
    return findSpecifications <BuildPhase> (domain);
}

BuildSystem::shared_ptr Manager::
buildSystem(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <BuildSystem> (domain, identifier);
}

BuildSystem::vector Manager::
buildSystems(std::string const &domain) const
{
    return findSpecifications <BuildSystem> (domain);
}

Compiler::shared_ptr Manager::
compiler(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <Compiler> (domain, identifier);
}

Compiler::vector Manager::
compilers(std::string const &domain) const
{
    return findSpecifications <Compiler> (domain);
}

FileType::shared_ptr Manager::
fileType(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <FileType> (domain, identifier);
}

FileType::vector Manager::
fileTypes(std::string const &domain) const
{
    return findSpecifications <FileType> (domain);
}

Linker::shared_ptr Manager::
linker(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <Linker> (domain, identifier);
}

Linker::vector Manager::
linkers(std::string const &domain) const
{
    return findSpecifications <Linker> (domain);
}

PackageType::shared_ptr Manager::
packageType(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <PackageType> (domain, identifier);
}

PackageType::vector Manager::
packageTypes(std::string const &domain) const
{
    return findSpecifications <PackageType> (domain);
}

ProductType::shared_ptr Manager::
productType(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <ProductType> (domain, identifier);
}

ProductType::vector Manager::
productTypes(std::string const &domain) const
{
    return findSpecifications <ProductType> (domain);
}

PropertyConditionFlavor::shared_ptr Manager::
propertyConditionFlavor(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <PropertyConditionFlavor> (domain, identifier);
}

PropertyConditionFlavor::vector Manager::
propertyConditionFlavors(std::string const &domain) const
{
    return findSpecifications <PropertyConditionFlavor> (domain);
}

Tool::shared_ptr Manager::
tool(std::string const &identifier, std::string const &domain) const
{
    return findSpecification <Tool> (domain, identifier);
}

Tool::vector Manager::
tools(std::string const &domain) const
{
    return findSpecifications <Tool> (domain);
}

static BuildRule::shared_ptr
SynthesizeBuildRule(Tool const *tool)
{

    return nullptr;
}

BuildRule::vector Manager::
synthesizedBuildRules(void) const
{
    BuildRule::vector buildRules;

    for (Compiler::shared_ptr const &compiler : compilers()) {
        if (compiler->synthesizeBuildRule()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(compiler->inputFileTypes(), compiler->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    for (Linker::shared_ptr const &linker : linkers()) {
        if (linker->synthesizeBuildRule()) {
            BuildRule::shared_ptr buildRule = std::make_shared <BuildRule> (BuildRule(linker->inputFileTypes(), linker->identifier()));
            buildRules.push_back(buildRule);
        }
    }

    for (Tool::shared_ptr const &tool : tools()) {
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

    if (auto ospec = specification(spec->type(), spec->identifier(), spec->domain(), spec->isDefault())) {
        if (ospec->isDefault() && spec->isDefault()) {
            fprintf(stderr, "error: registering %s specification '%s' in domain %s twice\n",
                    spec->type(), spec->identifier().c_str(), spec->domain().c_str());
            return;
        }
    }

#if 0
    fprintf(stderr, "adding %s spec to domain %s '%s'%s\n",
            spec->type(), spec->domain()).c_str(), spec->identifier().c_str(),
            spec->isDefault() ? "" : " [override]");
#endif
    _specifications[spec->domain()][spec->type()].push_back(spec);
}

void Manager::
registerDomain(std::string const &domain, std::string const &path)
{
    auto const &it = _domains.find(domain);
    if (it == _domains.end()) {
        Context context = {
            .manager = this,
            .domain = domain,
        };

        FSUtil::EnumerateRecursive(path, "*.xcspec",
                [&](std::string const &filename) -> bool
                {
                    if (!FSUtil::TestForDirectory(filename)) {
                        if (!Specification::Open(&context, filename)) {
                            fprintf(stderr, "warning: failed to import "
                                "specification '%s'\n", filename.c_str());
                        }
                    }
                    return true;
                });

        _domains.insert(std::make_pair(domain, path));
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

std::string Manager::
GlobalDomain(void)
{
    return "[global]";
}

Manager::shared_ptr Manager::
Create(void)
{
    return std::make_shared <Manager> ();
}

std::string Manager::
DeveloperSpecificationRoot(std::string const &developerRoot)
{
    return developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents";
}

std::string Manager::
DomainSpecificationRoot(std::string const &domainPath)
{
    // NOTE(grp): Some platforms have specifications in other directories besides the primary Specifications folder.
    return domainPath + "/Developer/Library/Xcode";
}

std::string Manager::
DeveloperBuildRules(std::string const &developerRoot)
{
    return developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents/Frameworks/DevToolsCore.framework/Versions/A/Resources/BuiltInBuildRules.plist";
}
