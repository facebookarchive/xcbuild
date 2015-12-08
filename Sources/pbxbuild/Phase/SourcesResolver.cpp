/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/Tool/ToolResolver.h>
#include <pbxbuild/Tool/ScriptResolver.h>
#include <pbxbuild/Tool/ClangResolver.h>
#include <pbxbuild/Tool/HeadermapResolver.h>
#include <pbxbuild/Tool/CompilationInfo.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <pbxbuild/Tool/ToolContext.h>

using pbxbuild::Phase::SourcesResolver;
using pbxbuild::Phase::PhaseEnvironment;
using pbxbuild::Tool::ToolResolver;
using pbxbuild::Tool::ClangResolver;
using pbxbuild::Tool::ScriptResolver;
using pbxbuild::Tool::HeadermapResolver;
using pbxbuild::Tool::CompilationInfo;
using pbxbuild::Tool::HeadermapInfo;
using pbxbuild::Tool::PrecompiledHeaderInfo;
using pbxbuild::Tool::SearchPaths;
using pbxbuild::Tool::ToolContext;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

SourcesResolver::
SourcesResolver(
    std::vector<ToolInvocation> const &invocations,
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> const &variantArchitectureInvocations,
    std::string const &linkerDriver,
    std::unordered_set<std::string> const &linkerArgs
) :
    _invocations                   (invocations),
    _variantArchitectureInvocations(variantArchitectureInvocations),
    _linkerDriver                  (linkerDriver),
    _linkerArgs                    (linkerArgs)
{
}

SourcesResolver::
~SourcesResolver()
{
}

static void
CreateCompilation(
    ClangResolver const &clangResolver,

    pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
    TypeResolvedFile const &file,
    pbxsetting::Environment const &environment,

    PhaseEnvironment const &phaseEnvironment,
    SearchPaths const &searchPaths,

    ToolContext *toolContext
)
{
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();
    std::string const &workingDirectory = targetEnvironment.workingDirectory();

    std::string outputBaseName;
    auto it = targetEnvironment.buildFileDisambiguation().find(buildFile);
    if (it != targetEnvironment.buildFileDisambiguation().end()) {
        outputBaseName = it->second;
    } else {
        outputBaseName = FSUtil::GetBaseNameWithoutExtension(file.filePath());
    }

    clangResolver.resolveSource(
        file,
        buildFile->compilerFlags(),
        outputBaseName,
        searchPaths,
        toolContext,
        environment,
        workingDirectory
    );
}

std::unique_ptr<SourcesResolver> SourcesResolver::
Create(
    pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment,
    pbxproj::PBX::SourcesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    ToolContext toolContext;

    std::unique_ptr<ScriptResolver> scriptResolver = ScriptResolver::Create(phaseEnvironment);
    if (scriptResolver == nullptr) {
        return nullptr;
    }

    std::unique_ptr<ClangResolver> clangResolver = ClangResolver::Create(phaseEnvironment);
    if (clangResolver == nullptr) {
        return nullptr;
    }

    std::unique_ptr<HeadermapResolver> headermapResolver = HeadermapResolver::Create(phaseEnvironment, clangResolver->compiler());
    if (headermapResolver == nullptr) {
        return nullptr;
    }

    std::string const &workingDirectory = targetEnvironment.workingDirectory();
    SearchPaths searchPaths = SearchPaths::Create(workingDirectory, targetEnvironment.environment());

    ToolInvocation headermapInvocation = headermapResolver->invocation(phaseEnvironment.target(), searchPaths, targetEnvironment.environment(), workingDirectory, &toolContext.headermapInfo());
    toolContext.invocations().push_back(headermapInvocation);

    std::unordered_map<pbxproj::PBX::BuildFile::shared_ptr, TypeResolvedFile> files;
    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
        if (buildFile->fileRef() == nullptr || buildFile->fileRef()->type() != pbxproj::PBX::GroupItem::kTypeFileReference) {
            continue;
        }

        pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
        std::unique_ptr<TypeResolvedFile> file = phaseEnvironment.resolveFileReference(fileReference, targetEnvironment.environment());
        if (file == nullptr) {
            continue;
        }

        files.insert({ buildFile, *file });
    }

    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment currentEnvironment = targetEnvironment.environment();
            currentEnvironment.insertFront(PhaseEnvironment::VariantLevel(variant), false);
            currentEnvironment.insertFront(PhaseEnvironment::ArchitectureLevel(arch), false);

            for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
                auto it = files.find(buildFile);
                if (it == files.end()) {
                    continue;
                }
                TypeResolvedFile const &file = it->second;

                pbxbuild::TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);
                if (buildRule != nullptr) {
                    if (buildRule->tool() != nullptr) {
                        pbxspec::PBX::Tool::shared_ptr tool = buildRule->tool();
                        if (tool->identifier() == "com.apple.compilers.gcc") {
                            CreateCompilation(
                                *clangResolver,

                                buildFile,
                                file,
                                currentEnvironment,

                                phaseEnvironment,
                                searchPaths,

                                &toolContext
                            );
                        } else {
                            // TODO(grp): Use an appropriate compiler context to create this invocation.
                            std::unique_ptr<ToolResolver> toolResolver = ToolResolver::Create(phaseEnvironment, tool->identifier());
                            ToolInvocation invocation = toolResolver->invocation({ }, { file.filePath() }, currentEnvironment, workingDirectory);
                            toolContext.invocations().push_back(invocation);
                        }
                    } else if (!buildRule->script().empty()) {
                        ToolInvocation invocation = scriptResolver->invocation(file.filePath(), buildRule, currentEnvironment, workingDirectory);
                        toolContext.invocations().push_back(invocation);
                    }
                } else {
                    fprintf(stderr, "warning: no matching build rule for %s (type %s)\n", file.filePath().c_str(), file.fileType()->identifier().c_str());
                    continue;
                }
            }
        }
    }

    return std::unique_ptr<SourcesResolver>(new SourcesResolver(toolContext.invocations(), toolContext.variantArchitectureInvocations(), toolContext.compilationInfo().linkerDriver(), toolContext.compilationInfo().linkerArguments()));
}
