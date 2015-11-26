/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/CopyInvocationContext.h>

using pbxbuild::Phase::ResourcesResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::CopyInvocationContext;
using pbxbuild::Tool::ToolInvocationContext;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
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

static ToolInvocation
CopyInvocation(pbxbuild::Phase::PhaseContext const &phaseContext, pbxspec::PBX::Tool::shared_ptr const &copyTool, TypeResolvedFile const &file, std::string const &outputDirectory, pbxsetting::Environment const &environment)
{
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseContext.targetEnvironment();
    std::string const &workingDirectory = targetEnvironment.workingDirectory();

    pbxbuild::TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);
    if (buildRule != nullptr && buildRule->tool() != nullptr) {
        pbxspec::PBX::Tool::shared_ptr tool = buildRule->tool();

        std::string outputPath = outputDirectory + "/" + FSUtil::GetBaseName(file.filePath());
        auto context = ToolInvocationContext::Create(tool, { file.filePath() }, { outputPath }, environment, workingDirectory);
        return context.invocation();
    } else {
        auto context = CopyInvocationContext::Create(copyTool, file.filePath(), outputDirectory, "CpResource", environment, workingDirectory);
        return context.invocation();
    }
}

std::unique_ptr<ResourcesResolver> ResourcesResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxsetting::Environment const &environment = phaseContext.targetEnvironment().environment();
    pbxspec::Manager::shared_ptr const &specManager = phaseContext.buildEnvironment().specManager();
    std::string resourcesDirectory = environment.resolve("BUILT_PRODUCTS_DIR") + "/" + environment.resolve("UNLOCALIZED_RESOURCES_FOLDER_PATH");

    pbxspec::PBX::Tool::shared_ptr copyTool = specManager->tool("com.apple.compilers.pbxcp", phaseContext.targetEnvironment().specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    std::vector<pbxbuild::ToolInvocation> invocations;

    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
        if (buildFile->fileRef() == nullptr) {
            continue;
        }

        switch (buildFile->fileRef()->type()) {
            case pbxproj::PBX::GroupItem::kTypeFileReference: {
                pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
                std::unique_ptr<TypeResolvedFile> file = phaseContext.resolveFileReference(fileReference, environment);
                if (file != nullptr) {
                    ToolInvocation invocation = CopyInvocation(phaseContext, copyTool, *file, resourcesDirectory, environment);
                    invocations.push_back(invocation);
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

                    std::unique_ptr<TypeResolvedFile> file = phaseContext.resolveFileReference(fileReference, environment);
                    if (file != nullptr) {
                        ToolInvocation invocation = CopyInvocation(phaseContext, copyTool, *file, outputDirectory, environment);
                        invocations.push_back(invocation);
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

    return std::unique_ptr<ResourcesResolver>(new ResourcesResolver(invocations));
}
