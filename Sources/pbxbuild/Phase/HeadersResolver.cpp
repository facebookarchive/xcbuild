/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Phase::HeadersResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::CopyResolver;
using pbxbuild::TypeResolvedFile;

HeadersResolver::
HeadersResolver(pbxproj::PBX::HeadersBuildPhase::shared_ptr const &buildPhase) :
    _buildPhase(buildPhase)
{
}

HeadersResolver::
~HeadersResolver()
{
}

bool HeadersResolver::
resolve(pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment, PhaseContext *phaseContext)
{
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();
    pbxsetting::Environment const &environment = targetEnvironment.environment();

    CopyResolver const *copyResolver = phaseContext->copyResolver(phaseEnvironment);
    if (copyResolver == nullptr) {
        return false;
    }

    std::string targetBuildDirectory = environment.resolve("TARGET_BUILD_DIR");
    std::string publicOutputDirectory = targetBuildDirectory + "/" + environment.resolve("PUBLIC_HEADERS_FOLDER_PATH");
    std::string privateOutputDirectory = targetBuildDirectory + "/" + environment.resolve("PRIVATE_HEADERS_FOLDER_PATH");

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : _buildPhase->files()) {
        if (buildFile->fileRef() == nullptr || buildFile->fileRef()->type() != pbxproj::PBX::GroupItem::kTypeFileReference) {
            continue;
        }

        pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
        std::unique_ptr<TypeResolvedFile> file = phaseEnvironment.resolveFileReference(fileReference, environment);
        if (file == nullptr) {
            continue;
        }

        std::vector<std::string> const &attributes = buildFile->attributes();
        bool isPublic  = std::find(attributes.begin(), attributes.end(), "Public") != attributes.end();
        bool isPrivate = std::find(attributes.begin(), attributes.end(), "Private") != attributes.end();

        std::string const &outputDirectory = (isPublic ? publicOutputDirectory : privateOutputDirectory);

        if (isPublic || isPrivate) {
            copyResolver->resolve(&phaseContext->toolContext(), environment, file->filePath(), outputDirectory, "CpHeader");
        }
    }

    return true;
}
