// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_PhaseContext_h
#define __pbxbuild_Phase_PhaseContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/TargetEnvironment.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext {
private:
    BuildEnvironment                 _buildEnvironment;
    BuildContext                     _buildContext;
    pbxproj::PBX::Target::shared_ptr _target;
    TargetEnvironment                _targetEnvironment;

public:
    PhaseContext(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target, TargetEnvironment const &targetEnvironment);
    ~PhaseContext();

public:
    BuildEnvironment const &buildEnvironment() const
    { return _buildEnvironment; }
    BuildContext const &buildContext() const
    { return _buildContext; }
    pbxproj::PBX::Target::shared_ptr const &target() const
    { return _target; }
    TargetEnvironment const &targetEnvironment() const
    { return _targetEnvironment; }

public:
    std::unique_ptr<pbxbuild::TypeResolvedFile> resolveReferenceProxy(pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy, pbxsetting::Environment const &environment) const;
    std::unique_ptr<pbxbuild::TypeResolvedFile> resolveFileReference(pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment) const;

public:
    static pbxsetting::Level
    VariantLevel(std::string const &variant);
    static pbxsetting::Level
    ArchitectureLevel(std::string const &arch);
};

}
}

#endif // !__pbxbuild_Phase_PhaseContext_h
