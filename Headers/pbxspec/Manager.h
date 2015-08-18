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

private:
    std::map<char const *, PBX::Specification::vector> _specifications;

public:
    Manager();
    ~Manager();

public:
    PBX::Specification::shared_ptr
    GetSpecification(char const *type, std::string const &identifier, bool onlyDefault = false) const;

public:
    PBX::Architecture::shared_ptr
    GetArchitecture(std::string const &identifier) const;
    PBX::BuildSystem::shared_ptr
    GetBuildSystem(std::string const &identifier) const;
    PBX::BuildPhase::shared_ptr
    GetBuildPhase(std::string const &identifier) const;
    PBX::Compiler::shared_ptr
    GetCompiler(std::string const &identifier) const;
    PBX::FileType::shared_ptr
    GetFileType(std::string const &identifier) const;
    PBX::Linker::shared_ptr
    GetLinker(std::string const &identifier) const;
    PBX::PackageType::shared_ptr
    GetPackageType(std::string const &identifier) const;
    PBX::ProductType::shared_ptr
    GetProductType(std::string const &identifier) const;
    PBX::PropertyConditionFlavor::shared_ptr
    GetPropertyConditionFlavor(std::string const &identifier) const;
    PBX::Tool::shared_ptr
    GetTool(std::string const &identifier) const;

public:
    pbxsetting::Level
    defaultSettings(void) const;

protected:
    friend class pbxspec::PBX::Specification;
    void
    AddSpecification(PBX::Specification::shared_ptr const &spec);

public:
    static Manager::shared_ptr
    Open(std::string const &path);
};

}

#endif  // !__pbxspec_Manager_h
