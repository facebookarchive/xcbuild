/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Phase::HeadersResolver;
using pbxbuild::Phase::PhaseEnvironment;
using pbxbuild::Tool::CopyResolver;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

HeadersResolver::
HeadersResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

HeadersResolver::
~HeadersResolver()
{
}

std::unique_ptr<HeadersResolver> HeadersResolver::
Create(
    pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment,
    pbxproj::PBX::HeadersBuildPhase::shared_ptr const &buildPhase
)
{
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();
    pbxsetting::Environment const &environment = targetEnvironment.environment();

    std::string targetBuildDirectory = environment.resolve("TARGET_BUILD_DIR");
    std::string publicOutputDirectory = targetBuildDirectory + "/" + environment.resolve("PUBLIC_HEADERS_FOLDER_PATH");
    std::string privateOutputDirectory = targetBuildDirectory + "/" + environment.resolve("PRIVATE_HEADERS_FOLDER_PATH");

    std::vector<pbxbuild::ToolInvocation> invocations;

    std::unique_ptr<CopyResolver> copyResolver = CopyResolver::Create(phaseEnvironment);
    if (copyResolver == nullptr) {
        return nullptr;
    }

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
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

        std::string const &filePath = file->filePath();
        std::string const &outputDirectory = (isPublic ? publicOutputDirectory : privateOutputDirectory);

        if (isPublic || isPrivate) {
            ToolInvocation invocation = copyResolver->invocation(filePath, outputDirectory, "CpHeader", environment, targetEnvironment.workingDirectory());
            invocations.push_back(invocation);
        }
    }

    return std::unique_ptr<HeadersResolver>(new HeadersResolver(invocations));
}
