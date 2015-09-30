// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/PhaseContext.h>

using pbxbuild::Phase::PhaseContext;

PhaseContext::
PhaseContext(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target, TargetEnvironment const &targetEnvironment) :
    _buildEnvironment (buildEnvironment),
    _buildContext     (buildContext),
    _target           (target),
    _targetEnvironment(targetEnvironment)
{
}

PhaseContext::
~PhaseContext()
{
}

std::unique_ptr<pbxbuild::TypeResolvedFile> PhaseContext::
resolveReferenceProxy(pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy, pbxsetting::Environment const &environment) const
{
    pbxproj::PBX::ContainerItemProxy::shared_ptr const &proxy = referenceProxy->remoteRef();
    pbxproj::PBX::FileReference::shared_ptr const &containerReference = proxy->containerPortal();
    std::string containerPath = environment.expand(containerReference->resolve());

    auto remote = _buildContext.resolveProductIdentifier(_buildContext.project(containerPath), proxy->remoteGlobalIDString());
    if (remote == nullptr) {
        fprintf(stderr, "error: unable to find remote target product from proxied reference\n");
        return nullptr;
    }

    std::unique_ptr<pbxbuild::TargetEnvironment> remoteEnvironment = _buildContext.targetEnvironment(_buildEnvironment, remote->first);
    if (remoteEnvironment == nullptr) {
        fprintf(stderr, "error: unable to create target environment for remote target\n");
        return nullptr;
    }

    return pbxbuild::TypeResolvedFile::Resolve(_buildEnvironment.specManager(), { pbxspec::Manager::AnyDomain() }, remote->second, remoteEnvironment->environment());
}

std::unique_ptr<pbxbuild::TypeResolvedFile> PhaseContext::
resolveFileReference(pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment) const
{
    return pbxbuild::TypeResolvedFile::Resolve(_buildEnvironment.specManager(), { pbxspec::Manager::AnyDomain() }, fileReference, environment);
}

pbxsetting::Level PhaseContext::
VariantLevel(std::string const &variant)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_VARIANT", variant),
        pbxsetting::Setting::Parse("variant", variant),
        pbxsetting::Setting::Parse("EXECUTABLE_VARIANT_SUFFIX", variant != "normal" ? "_" + variant : ""),
    });
}

pbxsetting::Level PhaseContext::
ArchitectureLevel(std::string const &arch)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_ARCH", arch),
        pbxsetting::Setting::Parse("arch", arch),
    });
}

