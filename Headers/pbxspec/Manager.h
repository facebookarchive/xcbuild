// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_Manager_h
#define __pbxspec_Manager_h

#include <pbxsetting/pbxsetting.h>
#include <pbxspec/PBX/Architecture.h>
#include <pbxspec/PBX/BuildPhase.h>
#include <pbxspec/PBX/BuildRule.h>
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

private:
    std::map<std::string, std::string>                                       _domains;
    std::map<std::string, std::map<char const*, PBX::Specification::vector>> _specifications;
    PBX::BuildRule::vector                                                   _buildRules;

public:
    Manager();
    ~Manager();

public:
    PBX::Specification::shared_ptr
    specification(char const *type, std::string const &identifier, std::string const &domain = GlobalDomain(), bool onlyDefault = false) const;
    PBX::Specification::vector
    specifications(char const *type, std::string const &domain = GlobalDomain()) const;

public:
    PBX::Architecture::shared_ptr
    architecture(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::Architecture::vector
    architectures(std::string const &domain = GlobalDomain()) const;

public:
    PBX::BuildSystem::shared_ptr
    buildSystem(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::BuildSystem::vector
    buildSystems(std::string const &domain = GlobalDomain()) const;

public:
    PBX::BuildPhase::shared_ptr
    buildPhase(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::BuildPhase::vector
    buildPhases(std::string const &domain = GlobalDomain()) const;

public:
    PBX::Compiler::shared_ptr
    compiler(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::Compiler::vector
    compilers(std::string const &domain = GlobalDomain()) const;

public:
    PBX::FileType::shared_ptr
    fileType(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::FileType::vector
    fileTypes(std::string const &domain = GlobalDomain()) const;

public:
    PBX::Linker::shared_ptr
    linker(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::Linker::vector
    linkers(std::string const &domain = GlobalDomain()) const;

public:
    PBX::PackageType::shared_ptr
    packageType(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::PackageType::vector
    packageTypes(std::string const &domain = GlobalDomain()) const;

public:
    PBX::ProductType::shared_ptr
    productType(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::ProductType::vector
    productTypes(std::string const &domain = GlobalDomain()) const;

public:
    PBX::PropertyConditionFlavor::shared_ptr
    propertyConditionFlavor(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::PropertyConditionFlavor::vector
    propertyConditionFlavors(std::string const &domain = GlobalDomain()) const;

public:
    PBX::Tool::shared_ptr
    tool(std::string const &identifier, std::string const &domain = GlobalDomain()) const;
    PBX::Tool::vector
    tools(std::string const &domain = GlobalDomain()) const;

public:
    inline PBX::BuildRule::vector buildRules(void) const
    { return _buildRules; }
    PBX::BuildRule::vector synthesizedBuildRules(void) const;

public:
    void
    registerDomain(std::string const &domain, std::string const &path);
    void
    registerBuildRules(std::string const &path);

protected:
    friend class pbxspec::PBX::Specification;
    void
    addSpecification(PBX::Specification::shared_ptr const &spec);

private:
    template <typename T>
    typename T::shared_ptr
    findSpecification(std::string const &identifier, std::string const &domain, char const *type = T::Type(), bool onlyDefault = false) const;
    template <typename T>
    typename T::vector
    findSpecifications(std::string const &domain, char const *type = T::Type()) const;

public:
    static Manager::shared_ptr
    Create(void);

public:
    static std::string
    GlobalDomain();

public:
    static std::string
    DeveloperSpecificationRoot(std::string const &developerRoot);
    static std::string
    DomainSpecificationRoot(std::string const &domainPath);
    static std::string
    DeveloperBuildRules(std::string const &developerRoot);
};

}

#endif  // !__pbxspec_Manager_h
