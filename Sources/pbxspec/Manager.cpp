// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/Manager.h>

using pbxspec::Manager;
using pbxspec::PBX::Specification;
using pbxspec::PBX::Architecture;
using pbxspec::PBX::BuildPhase;
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
findSpecifications(bool scoped, char const *type) const
{
    if (type == nullptr) {
        return typename T::vector();
    }

    typename T::vector specifications;

    if (!scoped) {
        if (Manager::shared_ptr parent = _parent.lock()) {
            typename T::vector parentSpecifications = parent->findSpecifications <T> (scoped, type);
            specifications.insert(specifications.end(), parentSpecifications.begin(), parentSpecifications.end());
        }
    }

    auto const &it = _specifications.find(type);
    if (it != _specifications.end()) {
        typename T::vector typeSpecifications = reinterpret_cast <typename T::vector const &> (it->second);
        specifications.insert(specifications.end(), typeSpecifications.begin(), typeSpecifications.end());
    }

    return specifications;
}

template <typename T>
typename T::shared_ptr Manager::
findSpecification(bool scoped, std::string const &identifier, char const *type, bool onlyDefault) const
{
    typename T::vector vector = findSpecifications <T> (scoped, type);

    //
    // Do an inverse find so that we can find the overrides.
    //
    auto I = std::find_if(
            vector.rbegin(), vector.rend(),
            [&identifier,onlyDefault](Specification::shared_ptr const &spec) -> bool
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
        if (I == vector.rend())
            return nullptr;
    }

    return *I;
}

Specification::shared_ptr Manager::
specification(char const *type, std::string const &identifier, bool onlyDefault, bool scoped) const
{
    return findSpecification <Specification> (scoped, identifier, type, onlyDefault);
}

Specification::vector Manager::
specifications(char const *type, bool scoped) const
{
    return findSpecifications <Specification> (scoped, type);
}

Architecture::shared_ptr Manager::
architecture(std::string const &identifier, bool scoped) const
{
    return findSpecification <Architecture> (scoped, identifier);
}

Architecture::vector Manager::
architectures(bool scoped) const
{
    return findSpecifications <Architecture> (scoped);
}

BuildPhase::shared_ptr Manager::
buildPhase(std::string const &identifier, bool scoped) const
{
    return findSpecification <BuildPhase> (scoped, identifier);
}

BuildPhase::vector Manager::
buildPhases(bool scoped) const
{
    return findSpecifications <BuildPhase> (scoped);
}

BuildSystem::shared_ptr Manager::
buildSystem(std::string const &identifier, bool scoped) const
{
    return findSpecification <BuildSystem> (scoped, identifier);
}

BuildSystem::vector Manager::
buildSystems(bool scoped) const
{
    return findSpecifications <BuildSystem> (scoped);
}

Compiler::shared_ptr Manager::
compiler(std::string const &identifier, bool scoped) const
{
    return findSpecification <Compiler> (scoped, identifier);
}

Compiler::vector Manager::
compilers(bool scoped) const
{
    return findSpecifications <Compiler> (scoped);
}

FileType::shared_ptr Manager::
fileType(std::string const &identifier, bool scoped) const
{
    return findSpecification <FileType> (scoped, identifier);
}

FileType::vector Manager::
fileTypes(bool scoped) const
{
    return findSpecifications <FileType> (scoped);
}

Linker::shared_ptr Manager::
linker(std::string const &identifier, bool scoped) const
{
    return findSpecification <Linker> (scoped, identifier);
}

Linker::vector Manager::
linkers(bool scoped) const
{
    return findSpecifications <Linker> (scoped);
}

PackageType::shared_ptr Manager::
packageType(std::string const &identifier, bool scoped) const
{
    return findSpecification <PackageType> (scoped, identifier);
}

PackageType::vector Manager::
packageTypes(bool scoped) const
{
    return findSpecifications <PackageType> (scoped);
}

ProductType::shared_ptr Manager::
productType(std::string const &identifier, bool scoped) const
{
    return findSpecification <ProductType> (scoped, identifier);
}

ProductType::vector Manager::
productTypes(bool scoped) const
{
    return findSpecifications <ProductType> (scoped);
}

PropertyConditionFlavor::shared_ptr Manager::
propertyConditionFlavor(std::string const &identifier, bool scoped) const
{
    return findSpecification <PropertyConditionFlavor> (scoped, identifier);
}

PropertyConditionFlavor::vector Manager::
propertyConditionFlavors(bool scoped) const
{
    return findSpecifications <PropertyConditionFlavor> (scoped);
}

Tool::shared_ptr Manager::
tool(std::string const &identifier, bool scoped) const
{
    return findSpecification <Tool> (scoped, identifier);
}

Tool::vector Manager::
tools(bool scoped) const
{
    return findSpecifications <Tool> (scoped);
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

    if (auto ospec = specification(spec->type(), spec->identifier(), spec->isDefault())) {
        if (ospec->isDefault() && spec->isDefault()) {
            fprintf(stderr, "error: registering %s specification '%s' twice\n",
                    spec->type(), spec->identifier().c_str());
            return;
        }
    }

#if 0
    fprintf(stderr, "adding %s spec '%s'%s\n",
            spec->type(), spec->identifier().c_str(),
            spec->isDefault() ? "" : " [override]");
#endif
    _specifications[spec->type()].push_back(spec);
}

Manager::shared_ptr Manager::
Open(Manager::shared_ptr parent, std::string const &path)
{
    Manager::shared_ptr manager = std::make_shared <Manager> ();
    manager->_parent = parent;

    FSUtil::EnumerateRecursive(path, "*.xcspec",
            [&](std::string const &filename) -> bool
            {
                if (!Specification::Open(manager, filename)) {
                    fprintf(stderr, "warning: failed to import "
                        "specification '%s'\n", filename.c_str());
                }
                return true;
            });

    return manager;
}

std::string Manager::
SpecificationRoot(std::string const &developerRoot)
{
    return developerRoot + "/../PlugIns/Xcode3Core.ideplugin/Contents";
}
