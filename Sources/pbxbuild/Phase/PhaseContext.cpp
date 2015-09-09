// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/PhaseContext.h>

using pbxbuild::Phase::PhaseContext;

PhaseContext::
PhaseContext(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, TargetEnvironment const &targetEnvironment) :
    _buildEnvironment (buildEnvironment),
    _buildContext     (buildContext),
    _targetEnvironment(targetEnvironment)
{
}

PhaseContext::
~PhaseContext()
{
}

std::unique_ptr<pbxbuild::TypeResolvedFile> PhaseContext::
resolveBuildFile(pbxsetting::Environment const &environment, pbxproj::PBX::BuildFile::shared_ptr const &buildFile) const
{
    if (pbxproj::PBX::FileReference::shared_ptr const &fileReference = buildFile->fileReference()) {
        std::string path = environment.expand(fileReference->resolve());
        return pbxbuild::TypeResolvedFile::Resolve(_buildEnvironment.specManager(), fileReference, environment);
    } else if (pbxproj::PBX::ReferenceProxy::shared_ptr referenceProxy = buildFile->referenceProxy()) {
        pbxproj::PBX::ContainerItemProxy::shared_ptr proxy = referenceProxy->remoteRef();
        pbxproj::PBX::FileReference::shared_ptr containerReference = proxy->containerPortal();
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

        return pbxbuild::TypeResolvedFile::Resolve(_buildEnvironment.specManager(), remote->second, remoteEnvironment->environment());
    } else {
        fprintf(stderr, "error: unable to handle build file without file reference or proxy\n");
        return nullptr;
    }
}

std::vector<pbxbuild::TypeResolvedFile> PhaseContext::
resolveBuildFiles(pbxsetting::Environment const &environment, std::vector<pbxproj::PBX::BuildFile::shared_ptr> const &buildFiles) const
{
    std::vector<pbxbuild::TypeResolvedFile> files;
    files.reserve(buildFiles.size());

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildFiles) {
        auto file = resolveBuildFile(environment, buildFile);
        if (file == nullptr) {
            continue;
        }
        files.push_back(*file);
    }

    return files;
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

