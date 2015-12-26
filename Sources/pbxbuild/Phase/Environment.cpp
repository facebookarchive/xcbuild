/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/Environment.h>

namespace Phase = pbxbuild::Phase;

Phase::Environment::
Environment(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target, Target::Environment const &targetEnvironment) :
    _buildEnvironment (buildEnvironment),
    _buildContext     (buildContext),
    _target           (target),
    _targetEnvironment(targetEnvironment)
{
}

Phase::Environment::
~Environment()
{
}

std::unique_ptr<pbxbuild::TypeResolvedFile> Phase::Environment::
resolveReferenceProxy(pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy, pbxsetting::Environment const &environment) const
{
    pbxproj::PBX::ContainerItemProxy::shared_ptr const &proxy = referenceProxy->remoteRef();
    pbxproj::PBX::FileReference::shared_ptr const &containerReference = proxy->containerPortal();
    std::string containerPath = environment.expand(containerReference->resolve());

    auto remote = _buildContext.resolveProductIdentifier(_buildContext.workspaceContext()->project(containerPath), proxy->remoteGlobalIDString());
    if (remote == nullptr) {
        fprintf(stderr, "error: unable to find remote target product from proxied reference\n");
        return nullptr;
    }

    std::unique_ptr<Target::Environment> remoteEnvironment = _buildContext.targetEnvironment(_buildEnvironment, remote->first);
    if (remoteEnvironment == nullptr) {
        fprintf(stderr, "error: unable to create target environment for remote target\n");
        return nullptr;
    }

    return pbxbuild::TypeResolvedFile::Resolve(_buildEnvironment.specManager(), { pbxspec::Manager::AnyDomain() }, remote->second, remoteEnvironment->environment());
}

std::unique_ptr<pbxbuild::TypeResolvedFile> Phase::Environment::
resolveFileReference(pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment) const
{
    return pbxbuild::TypeResolvedFile::Resolve(_buildEnvironment.specManager(), { pbxspec::Manager::AnyDomain() }, fileReference, environment);
}

pbxsetting::Level Phase::Environment::
VariantLevel(std::string const &variant)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_VARIANT", variant),
        pbxsetting::Setting::Parse("variant", variant),
        pbxsetting::Setting::Parse("EXECUTABLE_VARIANT_SUFFIX", variant != "normal" ? "_" + variant : ""),
    });
}

pbxsetting::Level Phase::Environment::
ArchitectureLevel(std::string const &arch)
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CURRENT_ARCH", arch),
        pbxsetting::Setting::Parse("arch", arch),
    });
}

