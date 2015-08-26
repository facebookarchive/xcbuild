// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_TargetEnvironment_h
#define __pbxbuild_TargetEnvironment_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>

namespace pbxbuild {

class BuildContext;

class TargetEnvironment {
private:
    BuildEnvironment                  _buildEnvironment;
    pbxproj::PBX::Target::shared_ptr  _target;
    BuildContext const               *_context;

private:
    xcsdk::SDK::Target::shared_ptr           _sdk;
    pbxspec::PBX::BuildSystem::shared_ptr    _buildSystem;
    pbxspec::PBX::ProductType::shared_ptr    _productType;
    pbxspec::PBX::PackageType::shared_ptr    _packageType;
    std::shared_ptr<pbxsetting::Environment> _environment;
    std::vector<std::string>                 _variants;
    std::vector<std::string>                 _architectures;

public:
    TargetEnvironment(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const *context);
    ~TargetEnvironment();

public:
    xcsdk::SDK::Target::shared_ptr const &sdk(void) const
    { return _sdk; }
    pbxspec::PBX::BuildSystem::shared_ptr const &buildSystem(void) const
    { return _buildSystem; }
    pbxspec::PBX::ProductType::shared_ptr const &productType(void) const
    { return _productType; }
    pbxspec::PBX::PackageType::shared_ptr const &packageType(void) const
    { return _packageType; }
    pbxsetting::Environment const &environment(void) const
    { return *_environment; }
    std::vector<std::string> const &variants(void) const
    { return _variants; }
    std::vector<std::string> const &architectures(void) const
    { return _architectures; }

public:
    static std::unique_ptr<TargetEnvironment>
    Create(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const *context);
};

}

#endif // !__pbxbuild_DependencyResolver_h
