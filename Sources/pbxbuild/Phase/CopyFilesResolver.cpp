/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/CopyInvocationContext.h>

using pbxbuild::Phase::CopyFilesResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::CopyInvocationContext;
using libutil::FSUtil;

CopyFilesResolver::
CopyFilesResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

CopyFilesResolver::
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

std::unique_ptr<CopyFilesResolver> CopyFilesResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::CopyFilesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseContext.targetEnvironment();
    pbxsetting::Environment const &environment = targetEnvironment.environment();

    pbxspec::PBX::Tool::shared_ptr copyTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.compilers.pbxcp", phaseContext.targetEnvironment().specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    std::string root = environment.expand(DestinationOutputPath(buildPhase->dstSubfolderSpec()));
    std::string path = environment.expand(buildPhase->dstPath());
    std::string outputDirectory = root + "/" + path;

    // TODO(grp): There are subtleties here involving encodings, stripping, deleting headers, etc.
    std::vector<pbxbuild::ToolInvocation> invocations;

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
        if (buildFile->fileRef() == nullptr) {
            continue;
        }

        std::unique_ptr<TypeResolvedFile> file = nullptr;

        switch (buildFile->fileRef()->type()) {
            case pbxproj::PBX::GroupItem::kTypeFileReference: {
                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                file = phaseContext.resolveFileReference(fileReference, environment);
                break;
            }
            case pbxproj::PBX::GroupItem::kTypeReferenceProxy: {
                pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy = std::static_pointer_cast <pbxproj::PBX::ReferenceProxy> (buildFile->fileRef());
                file = phaseContext.resolveReferenceProxy(referenceProxy, environment);
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

        auto context = CopyInvocationContext::Create(copyTool, file->filePath(), outputDirectory, "PBXCp", environment, targetEnvironment.workingDirectory());
        invocations.push_back(context.invocation());
    }

    return std::make_unique<CopyFilesResolver>(invocations);
}
