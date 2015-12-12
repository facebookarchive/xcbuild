/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseContext.h>

using pbxbuild::Phase::ResourcesResolver;
using pbxbuild::Phase::PhaseEnvironment;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::TypeResolvedFile;

ResourcesResolver::
ResourcesResolver(pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase) :
    _buildPhase(buildPhase)
{
}

ResourcesResolver::
~ResourcesResolver()
{
}

bool ResourcesResolver::
resolve(pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment, PhaseContext *phaseContext)
{
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();
    pbxspec::Manager::shared_ptr const &specManager = phaseEnvironment.buildEnvironment().specManager();
    std::string resourcesDirectory = environment.resolve("BUILT_PRODUCTS_DIR") + "/" + environment.resolve("UNLOCALIZED_RESOURCES_FOLDER_PATH");

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : _buildPhase->files()) {
        if (buildFile->fileRef() == nullptr) {
            continue;
        }

        switch (buildFile->fileRef()->type()) {
            case pbxproj::PBX::GroupItem::kTypeFileReference: {
                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                std::unique_ptr<TypeResolvedFile> file = phaseEnvironment.resolveFileReference(fileReference, environment);
                if (file != nullptr) {
                    if (!phaseContext->resolveBuildFile(phaseEnvironment, environment, _buildPhase, buildFile, *file, resourcesDirectory)) {
                        return false;
                    }
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
                    std::string outputDirectory = resourcesDirectory + "/" + fileReference->name() + ".lproj";

                    std::unique_ptr<TypeResolvedFile> file = phaseEnvironment.resolveFileReference(fileReference, environment);
                    if (file != nullptr) {
                        if (!phaseContext->resolveBuildFile(phaseEnvironment, environment, _buildPhase, buildFile, *file, outputDirectory)) {
                            return false;
                        }
                    }
                }
                break;
            }
            default: {
                fprintf(stderr, "warning: unsupported build file for resources build phase\n");
                break;
            }
        }
    }

    return true;
}
