/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>
#include <pbxbuild/Tool/CompilerInvocationContext.h>
#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>

using pbxbuild::Phase::SourcesResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::ToolInvocation;
using pbxbuild::Tool::ToolInvocationContext;
using pbxbuild::Tool::CompilerInvocationContext;
using pbxbuild::Tool::ScriptInvocationContext;
using pbxbuild::Tool::HeadermapInvocationContext;
using pbxbuild::Tool::PrecompiledHeaderInfo;
using pbxbuild::Tool::SearchPaths;
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

std::unique_ptr<SourcesResolver> SourcesResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::SourcesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseContext.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseContext.targetEnvironment();

    std::vector<ToolInvocation> allInvocations;
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> variantArchitectureInvocations;
    std::unordered_set<std::string> linkerArguments;

    pbxspec::PBX::Tool::shared_ptr scriptTool = buildEnvironment.specManager()->tool("com.apple.commands.shell-script", targetEnvironment.specDomains());
    pbxspec::PBX::Tool::shared_ptr headermapTool = buildEnvironment.specManager()->tool("com.apple.commands.built-in.headermap-generator", targetEnvironment.specDomains());

    // TODO(grp): This should probably try a number of other compilers if it's not clang.
    std::string gccVersion = targetEnvironment.environment().resolve("GCC_VERSION");
    pbxspec::PBX::Compiler::shared_ptr defaultCompiler = buildEnvironment.specManager()->compiler(gccVersion + ".compiler", targetEnvironment.specDomains());

    if (scriptTool == nullptr || headermapTool == nullptr || defaultCompiler == nullptr) {
        fprintf(stderr, "error: couldn't get compiler tools\n");
        return nullptr;
    }

    std::string linkerDriver = defaultCompiler->execPath();

    pbxsetting::Environment compilerEnvironment = targetEnvironment.environment();
    compilerEnvironment.insertFront(defaultCompiler->defaultSettings(), true);

    std::string workingDirectory = targetEnvironment.workingDirectory();
    SearchPaths searchPaths = SearchPaths::Create(workingDirectory, compilerEnvironment);

    HeadermapInvocationContext headermap = HeadermapInvocationContext::Create(headermapTool, buildEnvironment.specManager(), phaseContext.target(), searchPaths, compilerEnvironment, workingDirectory);
    allInvocations.push_back(headermap.invocation());

    bool precompilePrefixHeader = pbxsetting::Type::ParseBoolean(compilerEnvironment.resolve("GCC_PRECOMPILE_PREFIX_HEADER"));
    std::string prefixHeaderFile = compilerEnvironment.resolve("GCC_PREFIX_HEADER");
    std::unordered_map<std::string, ToolInvocation::AuxiliaryFile> prefixHeaderAuxiliaryFiles;

    std::unordered_map<pbxproj::PBX::BuildFile::shared_ptr, TypeResolvedFile> files;
    for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
        if (buildFile->fileRef() == nullptr || buildFile->fileRef()->type() != pbxproj::PBX::GroupItem::kTypeFileReference) {
            continue;
        }

        pbxproj::PBX::FileReference::shared_ptr const &fileReference = std::static_pointer_cast <pbxproj::PBX::FileReference> (buildFile->fileRef());
        std::unique_ptr<TypeResolvedFile> file = phaseContext.resolveFileReference(fileReference, targetEnvironment.environment());
        if (file == nullptr) {
            continue;
        }

        files.insert({ buildFile, *file });
    }

    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment currentEnvironment = targetEnvironment.environment();
            currentEnvironment.insertFront(PhaseContext::VariantLevel(variant), false);
            currentEnvironment.insertFront(PhaseContext::ArchitectureLevel(arch), false);

            std::vector<ToolInvocation> invocations;
            std::unordered_set<std::string> precompiledHeaders;

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
                            std::string const &dialect = file.fileType()->GCCDialectName();

                            if (dialect.size() > 2 && dialect.substr(dialect.size() - 2) == "++") {
                                linkerDriver = defaultCompiler->execCPlusPlusLinkerPath();
                            }

                            std::string outputBaseName;
                            auto it = targetEnvironment.buildFileDisambiguation().find(buildFile);
                            if (it != targetEnvironment.buildFileDisambiguation().end()) {
                                outputBaseName = it->second;
                            } else {
                                outputBaseName = FSUtil::GetBaseNameWithoutExtension(file.filePath());
                            }

                            auto context = CompilerInvocationContext::CreateSource(
                                defaultCompiler,
                                file,
                                buildFile->compilerFlags(),
                                outputBaseName,
                                headermap,
                                searchPaths,
                                currentEnvironment,
                                workingDirectory
                            );
                            invocations.push_back(context.invocation());
                            linkerArguments.insert(context.linkerArgs().begin(), context.linkerArgs().end());

                            if (context.precompiledHeaderInfo() != nullptr) {
                                PrecompiledHeaderInfo precompiledHeaderInfo = *context.precompiledHeaderInfo();
                                std::string hash = precompiledHeaderInfo.hash();

                                if (precompiledHeaders.find(hash) == precompiledHeaders.end()) {
                                    precompiledHeaders.insert(hash);

                                    auto precompiledHeaderContext = CompilerInvocationContext::CreatePrecompiledHeader(
                                        defaultCompiler,
                                        precompiledHeaderInfo,
                                        currentEnvironment,
                                        workingDirectory
                                    );
                                    invocations.push_back(precompiledHeaderContext.invocation());
                                }
                            }
                        } else {
                            // TODO(grp): Use an appropriate compiler context to create this invocation.
                            auto context = ToolInvocationContext::Create(tool, { }, { file.filePath() }, currentEnvironment, workingDirectory);
                            invocations.push_back(context.invocation());
                        }
                    } else if (!buildRule->script().empty()) {
                        auto context = ScriptInvocationContext::Create(scriptTool, file.filePath(), buildRule, currentEnvironment, workingDirectory);
                        invocations.push_back(context.invocation());
                    }
                } else {
                    fprintf(stderr, "warning: no matching build rule for %s (type %s)\n", file.filePath().c_str(), file.fileType()->identifier().c_str());
                    continue;
                }
            }

            allInvocations.insert(allInvocations.end(), invocations.begin(), invocations.end());

            std::pair<std::string, std::string> variantArchitectureKey = std::make_pair(variant, arch);
            variantArchitectureInvocations.insert({ variantArchitectureKey, invocations });
        }
    }

    return std::unique_ptr<SourcesResolver>(new SourcesResolver(allInvocations, variantArchitectureInvocations, linkerDriver, linkerArguments));
}
