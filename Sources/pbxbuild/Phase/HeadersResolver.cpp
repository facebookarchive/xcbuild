// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/CopyInvocationContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Phase::HeadersResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::CopyInvocationContext;
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
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::HeadersBuildPhase::shared_ptr const &buildPhase
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
    std::string targetBuildDirectory = environment.resolve("TARGET_BUILD_DIR");
    std::string publicOutputDirectory = targetBuildDirectory + "/" + environment.resolve("PUBLIC_HEADERS_FOLDER_PATH");
    std::string privateOutputDirectory = targetBuildDirectory + "/" + environment.resolve("PRIVATE_HEADERS_FOLDER_PATH");

    std::vector<pbxbuild::ToolInvocation> invocations;

    for (auto const &entry : buildFiles) {
        std::vector<std::string> const &attributes = entry.first->attributes();
        bool isPublic  = std::find(attributes.begin(), attributes.end(), "Public") != attributes.end();
        bool isPrivate = std::find(attributes.begin(), attributes.end(), "Private") != attributes.end();

        std::string const &filePath = entry.second.filePath();
        std::string const &outputDirectory = (isPublic ? publicOutputDirectory : privateOutputDirectory);

        if (isPublic || isPrivate) {
            auto context = CopyInvocationContext::Create(copyTool, filePath, outputDirectory, "CpHeader", environment, targetEnvironment.workingDirectory());
            invocations.push_back(context.invocation());
        }
    }

    return std::make_unique<HeadersResolver>(invocations);
}
