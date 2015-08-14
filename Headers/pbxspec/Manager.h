// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_Manager_h
#define __pbxspec_Manager_h

#include <pbxspec/PBX/Architecture.h>
#include <pbxspec/PBX/BuildPhase.h>
#include <pbxspec/PBX/BuildSystem.h>
#include <pbxspec/PBX/Compiler.h>
#include <pbxspec/PBX/FileType.h>
#include <pbxspec/PBX/Linker.h>
#include <pbxspec/PBX/PackageType.h>
#include <pbxspec/PBX/ProductType.h>
#include <pbxspec/PBX/PropertyConditionFlavor.h>
#include <pbxspec/PBX/Tool.h>

namespace pbxspec {

class Manager {
public:
    static PBX::Specification::shared_ptr GetSpecification(char const *type,
            std::string const &identifier, bool onlyDefault = false);

public:
    static PBX::Architecture::shared_ptr GetArchitecture(std::string const &identifier);
    static PBX::BuildSystem::shared_ptr GetBuildSystem(std::string const &identifier);
    static PBX::BuildPhase::shared_ptr GetBuildPhase(std::string const &identifier);
    static PBX::Compiler::shared_ptr GetCompiler(std::string const &identifier);
    static PBX::FileType::shared_ptr GetFileType(std::string const &identifier);
    static PBX::Linker::shared_ptr GetLinker(std::string const &identifier);
    static PBX::PackageType::shared_ptr GetPackageType(std::string const &identifier);
    static PBX::ProductType::shared_ptr GetProductType(std::string const &identifier);
    static PBX::PropertyConditionFlavor::shared_ptr GetPropertyConditionFlavor(std::string const &identifier);
    static PBX::Tool::shared_ptr GetTool(std::string const &identifier);

public:
    static bool Open(std::string const &filename);
    static void Import(std::string const &path);

protected:
    friend class pbxspec::PBX::Specification;
    static void AddSpecification(PBX::Specification::shared_ptr const &spec);
};

}

#endif  // !__pbxspec_Manager_h
