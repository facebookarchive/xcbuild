/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/File.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Target/Environment.h>
#include <pbxbuild/Build/Context.h>
#include <pbxbuild/Build/Environment.h>
#include <pbxbuild/TypeResolvedFile.h>

namespace Phase = pbxbuild::Phase;
namespace Target = pbxbuild::Target;
namespace Build = pbxbuild::Build;
using pbxbuild::TypeResolvedFile;

Phase::File::
File(pbxproj::PBX::BuildFile::shared_ptr const &buildFile, TypeResolvedFile const &file, std::string const &outputSubdirectory, std::string const &fileNameDisambiguator) :
    _buildFile            (buildFile),
    _file                 (file),
    _outputSubdirectory   (outputSubdirectory),
    _fileNameDisambiguator(fileNameDisambiguator)
{
}

Phase::File::
~File()
{
}

std::unique_ptr<pbxbuild::TypeResolvedFile> Phase::File::
ResolveReferenceProxy(Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Build::Context const &buildContext = phaseEnvironment.buildContext();

    pbxproj::PBX::ContainerItemProxy::shared_ptr const &proxy = referenceProxy->remoteRef();
    pbxproj::PBX::FileReference::shared_ptr const &containerReference = proxy->containerPortal();
    std::string containerPath = environment.expand(containerReference->resolve());

    auto remote = buildContext.resolveProductIdentifier(buildContext.workspaceContext()->project(containerPath), proxy->remoteGlobalIDString());
    if (remote == nullptr) {
        fprintf(stderr, "error: unable to find remote target product from proxied reference\n");
        return nullptr;
    }

    std::unique_ptr<Target::Environment> remoteEnvironment = buildContext.targetEnvironment(buildEnvironment, remote->first);
    if (remoteEnvironment == nullptr) {
        fprintf(stderr, "error: unable to create target environment for remote target\n");
        return nullptr;
    }

    return TypeResolvedFile::Resolve(buildEnvironment.specManager(), { pbxspec::Manager::AnyDomain() }, remote->second, remoteEnvironment->environment());
}

std::unique_ptr<pbxbuild::TypeResolvedFile> Phase::File::
ResolveFileReference(Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, pbxproj::PBX::FileReference::shared_ptr const &fileReference)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    return TypeResolvedFile::Resolve(buildEnvironment.specManager(), { pbxspec::Manager::AnyDomain() }, fileReference, environment);
}

std::vector<Phase::File> Phase::File::
ResolveBuildFiles(Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, std::vector<pbxproj::PBX::BuildFile::shared_ptr> const &buildFiles)
{
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    std::vector<Phase::File> result;

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildFiles) {
        if (buildFile->fileRef() == nullptr) {
            continue;
        }

        std::string fileNameDisambiguator;
        auto it = targetEnvironment.buildFileDisambiguation().find(buildFile);
        if (it != targetEnvironment.buildFileDisambiguation().end()) {
            fileNameDisambiguator = it->second;
        }

        switch (buildFile->fileRef()->type()) {
            case pbxproj::PBX::GroupItem::kTypeFileReference: {
                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                std::unique_ptr<TypeResolvedFile> file = ResolveFileReference(phaseEnvironment, environment, fileReference);
                if (file != nullptr) {
                    result.push_back(Phase::File(buildFile, *file, std::string(), fileNameDisambiguator));
                }
                break;
            }
            case pbxproj::PBX::GroupItem::kTypeReferenceProxy: {
                pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy = std::static_pointer_cast <pbxproj::PBX::ReferenceProxy> (buildFile->fileRef());
                std::unique_ptr<TypeResolvedFile> file = ResolveReferenceProxy(phaseEnvironment, environment, referenceProxy);
                if (file != nullptr) {
                    result.push_back(Phase::File(buildFile, *file, std::string(), fileNameDisambiguator));
                }
                break;
            }
            case pbxproj::PBX::GroupItem::kTypeVariantGroup: {
                pbxproj::PBX::VariantGroup::shared_ptr const &variantGroup = std::static_pointer_cast <pbxproj::PBX::VariantGroup> (buildFile->fileRef());
                for (pbxproj::PBX::GroupItem::shared_ptr const &child : variantGroup->children()) {
                    if (child->type() != pbxproj::PBX::GroupItem::kTypeFileReference) {
                        continue;
                    }

                    pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (child);
                    std::string outputDirectory = fileReference->name() + ".lproj";

                    std::unique_ptr<TypeResolvedFile> file = ResolveFileReference(phaseEnvironment, environment, fileReference);
                    if (file != nullptr) {
                        result.push_back(Phase::File(buildFile, *file, outputDirectory, fileNameDisambiguator));
                    }
                }
                break;
            }
            case pbxproj::PBX::GroupItem::kTypeGroup:
            case pbxproj::PBX::GroupItem::kTypeVersionGroup: {
                fprintf(stderr, "warning: unhandled group item type\n");
                break;
            }
        }
    }

    return result;
}
