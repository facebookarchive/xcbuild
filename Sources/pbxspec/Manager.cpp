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

namespace {

typedef std::map <char const *, Specification::vector> spec_map;

}

static spec_map s_Specifications;

template <typename T>
static typename T::shared_ptr
FindSpecification(std::string const &identifier,
        char const *type = T::Type(),
        bool onlyDefault = false)
{
    if (type == nullptr)
        return nullptr;

    auto const &vector = s_Specifications[type];

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

    return reinterpret_cast <typename T::shared_ptr const &> (*I);
}

Specification::shared_ptr Manager::
GetSpecification(char const *type, std::string const &identifier,
        bool onlyDefault)
{
    return FindSpecification <Specification> (identifier, type, onlyDefault);
}

Architecture::shared_ptr Manager::
GetArchitecture(std::string const &identifier)
{
    return FindSpecification <Architecture> (identifier);
}

BuildPhase::shared_ptr Manager::
GetBuildPhase(std::string const &identifier)
{
    return FindSpecification <BuildPhase> (identifier);
}

BuildSystem::shared_ptr Manager::
GetBuildSystem(std::string const &identifier)
{
    return FindSpecification <BuildSystem> (identifier);
}

Compiler::shared_ptr Manager::
GetCompiler(std::string const &identifier)
{
    return FindSpecification <Compiler> (identifier);
}

FileType::shared_ptr Manager::
GetFileType(std::string const &identifier)
{
    return FindSpecification <FileType> (identifier);
}

Linker::shared_ptr Manager::
GetLinker(std::string const &identifier)
{
    return FindSpecification <Linker> (identifier);
}

PackageType::shared_ptr Manager::
GetPackageType(std::string const &identifier)
{
    return FindSpecification <PackageType> (identifier);
}

ProductType::shared_ptr Manager::
GetProductType(std::string const &identifier)
{
    return FindSpecification <ProductType> (identifier);
}

PropertyConditionFlavor::shared_ptr Manager::
GetPropertyConditionFlavor(std::string const &identifier)
{
    return FindSpecification <PropertyConditionFlavor> (identifier);
}

Tool::shared_ptr Manager::
GetTool(std::string const &identifier)
{
    return FindSpecification <Tool> (identifier);
}

void Manager::
AddSpecification(PBX::Specification::shared_ptr const &spec)
{
    if (!spec) {
        fprintf(stderr, "error: registering null specification\n");
        return;
    }

    if (spec->type() == nullptr) {
        fprintf(stderr, "error: registering a specification with null type\n");
        return;
    }

    if (auto ospec = GetSpecification(spec->type(), spec->identifier(), spec->isDefault())) {
        if (ospec->isDefault() && spec->isDefault()) {
            fprintf(stderr, "error: registering %s specification '%s' twice\n",
                    spec->type(), spec->identifier().c_str());
            return;
        }
    }

    fprintf(stderr, "adding %s spec '%s'%s\n",
            spec->type(), spec->identifier().c_str(),
            spec->isDefault() ? "" : " [override]");
    s_Specifications[spec->type()].push_back(spec);
}

bool Manager::
Open(std::string const &filename)
{
    return Specification::Open(filename);
}

void Manager::
Import(std::string const &path)
{
    FSUtil::EnumerateDirectory(path, "*.xcspec",
            [&](std::string const &filename) -> bool
            {
                std::string fullPath = path + "/" + filename;
                if (!Open(fullPath)) {
                    fprintf(stderr, "warning: failed to import "
                        "specification '%s'\n", fullPath.c_str());
                }
                return true;
            });
}
