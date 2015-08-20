// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_Manager_h
#define __pbxspec_Manager_h

#include <pbxsetting/pbxsetting.h>
#include <pbxspec/PBX/Architecture.h>
#include <pbxspec/PBX/BuildPhase.h>
#include <pbxspec/PBX/BuildSystem.h>
#include <pbxspec/PBX/Compiler.h>
#include <pbxspec/PBX/FileType.h>
#include <pbxspec/PBX/Linker.h>
#include <pbxspec/PBX/PackageType.h>
#include <pbxspec/PBX/ProductType.h>
#include <pbxspec/PBX/PropertyConditionFlavor.h>
#include <pbxspec/PBX/Specification.h>
#include <pbxspec/PBX/Tool.h>

namespace pbxspec {

class Manager {
public:
    typedef std::shared_ptr <Manager> shared_ptr;
    typedef std::weak_ptr <Manager> weak_ptr;

private:
    Manager::weak_ptr _parent;
    std::map<char const *, PBX::Specification::vector> _specifications;

public:
    Manager();
    ~Manager();

public:
    Manager::shared_ptr parent()
    { return _parent.lock(); }

public:
    PBX::Specification::shared_ptr
    specification(char const *type, std::string const &identifier, bool onlyDefault = false, bool scoped = false) const;
    PBX::Specification::vector
    specifications(char const *type, bool scoped = false) const;

public:
    PBX::Architecture::shared_ptr
    architecture(std::string const &identifier, bool scoped = false) const;
    PBX::Architecture::vector
    architectures(bool scoped = false) const;

public:
    PBX::BuildSystem::shared_ptr
    buildSystem(std::string const &identifier, bool scoped = false) const;
    PBX::BuildSystem::vector
    buildSystems(bool scoped = false) const;

public:
    PBX::BuildPhase::shared_ptr
    buildPhase(std::string const &identifier, bool scoped = false) const;
    PBX::BuildPhase::vector
    buildPhases(bool scoped = false) const;

public:
    PBX::Compiler::shared_ptr
    compiler(std::string const &identifier, bool scoped = false) const;
    PBX::Compiler::vector
    compilers(bool scoped = false) const;

public:
    PBX::FileType::shared_ptr
    fileType(std::string const &identifier, bool scoped = false) const;
    PBX::FileType::vector
    fileTypes(bool scoped = false) const;

public:
    PBX::Linker::shared_ptr
    linker(std::string const &identifier, bool scoped = false) const;
    PBX::Linker::vector
    linkers(bool scoped = false) const;

public:
    PBX::PackageType::shared_ptr
    packageType(std::string const &identifier, bool scoped = false) const;
    PBX::PackageType::vector
    packageTypes(bool scoped = false) const;

public:
    PBX::ProductType::shared_ptr
    productType(std::string const &identifier, bool scoped = false) const;
    PBX::ProductType::vector
    productTypes(bool scoped = false) const;

public:
    PBX::PropertyConditionFlavor::shared_ptr
    propertyConditionFlavor(std::string const &identifier, bool scoped = false) const;
    PBX::PropertyConditionFlavor::vector
    propertyConditionFlavors(bool scoped = false) const;

public:
    PBX::Tool::shared_ptr
    tool(std::string const &identifier, bool scoped = false) const;
    PBX::Tool::vector
    tools(bool scoped = false) const;

protected:
    friend class pbxspec::PBX::Specification;
    void
    addSpecification(PBX::Specification::shared_ptr const &spec);

private:
    template <typename T>
    typename T::shared_ptr
    findSpecification(bool scoped, std::string const &identifier, char const *type = T::Type(), bool onlyDefault = false) const;
    template <typename T>
    typename T::vector
    findSpecifications(bool scoped, char const *type = T::Type()) const;

public:
    static Manager::shared_ptr
    Open(Manager::shared_ptr parent, std::string const &path);
};

}

#endif  // !__pbxspec_Manager_h
