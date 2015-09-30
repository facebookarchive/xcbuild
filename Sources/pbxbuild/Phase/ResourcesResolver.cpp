// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/CopyInvocationContext.h>

using pbxbuild::Phase::ResourcesResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::CopyInvocationContext;
using libutil::FSUtil;

ResourcesResolver::
ResourcesResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

ResourcesResolver::
~ResourcesResolver()
{
}

std::unique_ptr<ResourcesResolver> ResourcesResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseContext.targetEnvironment();
    pbxsetting::Environment const &environment = targetEnvironment.environment();

    pbxspec::PBX::Tool::shared_ptr copyTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.compilers.pbxcp", phaseContext.targetEnvironment().specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    std::string outputDirectory = environment.resolve("UNLOCALIZED_RESOURCES_FOLDER_PATH");

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
            case pbxproj::PBX::GroupItem::kTypeVariantGroup: {
                pbxproj::PBX::VariantGroup::shared_ptr const &variantGroup = std::static_pointer_cast <pbxproj::PBX::VariantGroup> (buildFile->fileRef());
                // TODO(grp): Handle variant group resources, for localized resources.
                break;
            }
            default: {
                fprintf(stderr, "warning: unsupported build file for resources build phase\n");
                break;
            }
        }

        if (file == nullptr) {
            continue;
        }

        // TODO(grp): Apply build rules for some types of resources.

        auto context = CopyInvocationContext::Create(copyTool, file->filePath(), outputDirectory, "CpResource", environment, targetEnvironment.workingDirectory());
        invocations.push_back(context.invocation());
    }

    return std::make_unique<ResourcesResolver>(invocations);
}
