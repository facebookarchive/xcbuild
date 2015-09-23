// Copyright 2013-present Facebook. All Rights Reserved.

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
    switch (destination) {
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationAbsolute:
            return pbxsetting::Value::Variable("BUILT_PRODUCTS_DIR");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationWrapper:
            return pbxsetting::Value::Variable("CONTENTS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationExecutables:
            return pbxsetting::Value::Variable("EXECUTABLE_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationResources:
            return pbxsetting::Value::Variable("UNLOCALIZED_RESOURCES_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationPublicHeaders:
            return pbxsetting::Value::Variable("PUBLIC_HEADERS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationPrivateHeaders:
            return pbxsetting::Value::Variable("PRIVATE_HEADERS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationFrameworks:
            return pbxsetting::Value::Variable("FRAMEWORKS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationSharedFrameworks:
            return pbxsetting::Value::Variable("SHARED_FRAMEWORKS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationSharedSupport:
            return pbxsetting::Value::Variable("SHARED_SUPPORT_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationPlugIns:
            return pbxsetting::Value::Variable("PLUGINS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationScripts:
            return pbxsetting::Value::Variable("SCRIPTS_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationJavaResources:
            return pbxsetting::Value::Variable("JAVA_FOLDER_PATH");
        case pbxproj::PBX::CopyFilesBuildPhase::kDestinationProducts:
            return pbxsetting::Value::Variable("BUILT_PRODUCTS_DIR");
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

    auto buildFiles = phaseContext.resolveBuildFiles(environment, buildPhase->files());

    std::string root = environment.expand(DestinationOutputPath(buildPhase->dstSubfolderSpec()));
    std::string path = environment.expand(pbxsetting::Value::Parse(buildPhase->dstPath()));
    std::string outputDirectory = root + "/" + path;

    // TODO(grp): There are subtleties here involving encodings, stripping, deleting headers, etc.
    std::vector<pbxbuild::ToolInvocation> invocations;

    for (auto const &entry : buildFiles) {
        std::string const &filePath = entry.second.filePath();

        auto context = CopyInvocationContext::Create(copyTool, filePath, outputDirectory, "PBXCp", environment, targetEnvironment.workingDirectory());
        invocations.push_back(context.invocation());
    }

    return std::make_unique<CopyFilesResolver>(invocations);
}
