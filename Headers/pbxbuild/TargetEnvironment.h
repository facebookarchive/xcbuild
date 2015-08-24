// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_TargetEnvironment_h
#define __pbxbuild_TargetEnvironment_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>

namespace pbxbuild {

class TargetEnvironment {
private:
    BuildEnvironment                 _buildEnvironment;
    pbxproj::PBX::Target::shared_ptr _target;
    BuildContext                     _context;

private:
    xcsdk::SDK::Target::shared_ptr           _sdk;
    pbxspec::PBX::BuildSystem::shared_ptr    _buildSystem;
    pbxspec::PBX::ProductType::shared_ptr    _productType;
    pbxspec::PBX::PackageType::shared_ptr    _packageType;
    std::shared_ptr<pbxsetting::Environment> _environment;

public:
    TargetEnvironment(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const &context);
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

public:
    static std::unique_ptr<TargetEnvironment>
    Create(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target, BuildContext const &context);
};

}

#endif // !__pbxbuild_DependencyResolver_h
