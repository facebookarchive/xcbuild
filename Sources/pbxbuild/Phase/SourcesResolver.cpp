// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/TypeResolvedFile.h>
#include <pbxbuild/TargetEnvironment.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/ScriptInvocationContext.h>
#include <pbxbuild/CompilerInvocationContext.h>
#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/Tool/SearchPaths.h>

using pbxbuild::Phase::SourcesResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::ToolInvocation;
using pbxbuild::CompilerInvocationContext;
using pbxbuild::ToolInvocationContext;
using pbxbuild::Tool::HeadermapInvocationContext;
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

    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment currentEnvironment = targetEnvironment.environment();
            currentEnvironment.insertFront(PhaseContext::VariantLevel(variant), false);
            currentEnvironment.insertFront(PhaseContext::ArchitectureLevel(arch), false);

            std::vector<ToolInvocation> invocations;
            std::unordered_map<std::string, std::string> prefixHeaders;

            auto files = phaseContext.resolveBuildFiles(targetEnvironment.environment(), buildPhase->files());
            for (auto const &fileEntry : files) {
                pbxproj::PBX::BuildFile::shared_ptr const &buildFile = fileEntry.first;
                pbxbuild::TypeResolvedFile const &file = fileEntry.second;

                pbxbuild::TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);
                if (buildRule != nullptr) {
                    if (buildRule->tool() != nullptr) {
                        pbxspec::PBX::Tool::shared_ptr tool = buildRule->tool();
                        if (tool->identifier() == "com.apple.compilers.gcc") {
                            std::string const &dialect = file.fileType()->GCCDialectName();

                            std::string prefixHeader;
                            if (!prefixHeaderFile.empty()) {
                                if (precompilePrefixHeader) {
                                    auto it = prefixHeaders.find(dialect);
                                    if (it != prefixHeaders.end()) {
                                        prefixHeader = it->second;
                                    } else {
                                        auto context = CompilerInvocationContext::CreatePrecompiledHeader(
                                            defaultCompiler,
                                            prefixHeaderFile,
                                            file.fileType(),
                                            headermap,
                                            searchPaths,
                                            currentEnvironment,
                                            workingDirectory
                                        );
                                        allInvocations.push_back(context.invocation());

                                        prefixHeader = context.output();
                                        prefixHeaders.insert({ dialect, prefixHeader });
                                    }
                                } else {
                                    prefixHeader = workingDirectory + "/" + prefixHeaderFile;
                                }
                            }

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
                                prefixHeader,
                                headermap,
                                searchPaths,
                                currentEnvironment,
                                workingDirectory
                            );
                            invocations.push_back(context.invocation());
                            linkerArguments.insert(context.linkerArgs().begin(), context.linkerArgs().end());
                        } else {
                            // TODO(grp): Use an appropriate compiler context to create this invocation.
                            auto context = ToolInvocationContext::Create(tool, { }, { file.filePath() }, currentEnvironment, workingDirectory);
                            invocations.push_back(context.invocation());
                        }
                    } else if (!buildRule->script().empty()) {
                        auto context = pbxbuild::ScriptInvocationContext::Create(scriptTool, file.filePath(), buildRule, currentEnvironment, workingDirectory);
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

    return std::make_unique<SourcesResolver>(allInvocations, variantArchitectureInvocations, linkerDriver, linkerArguments);
}
