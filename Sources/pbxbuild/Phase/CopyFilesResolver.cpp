/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/CopyResolver.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Phase::CopyFilesResolver::
CopyFilesResolver(pbxproj::PBX::CopyFilesBuildPhase::shared_ptr const &buildPhase) :
    _buildPhase(buildPhase)
{
}

Phase::CopyFilesResolver::
~CopyFilesResolver()
{
}

static pbxsetting::Value
DestinationOutputPath(pbxproj::PBX::CopyFilesBuildPhase::Destination destination)
{
    pbxsetting::Value products = pbxsetting::Value::Variable("BUILT_PRODUCTS_DIR") + pbxsetting::Value::String("/");

    switch (destination) {
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationAbsolute:
            return pbxsetting::Value::Empty();
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationWrapper:
            return products + pbxsetting::Value::Variable("CONTENTS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationExecutables:
            return products + pbxsetting::Value::Variable("EXECUTABLE_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationResources:
            return products + pbxsetting::Value::Variable("UNLOCALIZED_RESOURCES_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationPublicHeaders:
            return products + pbxsetting::Value::Variable("PUBLIC_HEADERS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationPrivateHeaders:
            return products + pbxsetting::Value::Variable("PRIVATE_HEADERS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationFrameworks:
            return products + pbxsetting::Value::Variable("FRAMEWORKS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationSharedFrameworks:
            return products + pbxsetting::Value::Variable("SHARED_FRAMEWORKS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationSharedSupport:
            return products + pbxsetting::Value::Variable("SHARED_SUPPORT_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationPlugIns:
            return products + pbxsetting::Value::Variable("PLUGINS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationScripts:
            return products + pbxsetting::Value::Variable("SCRIPTS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationJavaResources:
            return products + pbxsetting::Value::Variable("JAVA_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationProducts:
            return products;
        default:
            fprintf(stderr, "error: unknown destination %d\n", (int)destination);
            return pbxsetting::Value::Empty();
    }
}

bool Phase::CopyFilesResolver::
resolve(Phase::PhaseEnvironment const &phaseEnvironment, Phase::PhaseContext *phaseContext)
{
    Target::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();
    pbxsetting::Environment const &environment = targetEnvironment.environment();

    Tool::CopyResolver const *copyResolver = phaseContext->copyResolver(phaseEnvironment);
    if (copyResolver == nullptr) {
        return false;
    }

    std::string root = environment.expand(DestinationOutputPath(_buildPhase->dstSubfolderSpec()));
    std::string path = environment.expand(_buildPhase->dstPath());
    std::string outputDirectory = root + "/" + path;

    // TODO(grp): There are subtleties here involving encodings, stripping, deleting headers, etc.
    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : _buildPhase->files()) {
        if (buildFile->fileRef() == nullptr) {
            continue;
        }

        std::unique_ptr<TypeResolvedFile> file = nullptr;

        switch (buildFile->fileRef()->type()) {
            case pbxproj::PBX::GroupItem::kTypeFileReference: {
                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                file = phaseEnvironment.resolveFileReference(fileReference, environment);
                break;
            }
            case pbxproj::PBX::GroupItem::kTypeReferenceProxy: {
                pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy = std::static_pointer_cast <pbxproj::PBX::ReferenceProxy> (buildFile->fileRef());
                file = phaseEnvironment.resolveReferenceProxy(referenceProxy, environment);
                break;
            }
            default: {
                fprintf(stderr, "warning: unsupported build file for copy files build phase\n");
                break;
            }
        }

        if (file == nullptr) {
            continue;
        }

        copyResolver->resolve(&phaseContext->toolContext(), environment, file->filePath(), outputDirectory, "PBXCp");
    }

    return true;
}
