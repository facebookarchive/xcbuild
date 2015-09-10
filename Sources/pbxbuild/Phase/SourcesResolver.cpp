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

using pbxbuild::Phase::SourcesResolver;
using pbxbuild::Phase::PhaseContext;
using libutil::FSUtil;

SourcesResolver::
SourcesResolver(std::vector<pbxbuild::ToolInvocation> const &invocations, std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> const &variantArchitectureInvocations) :
    _invocations                   (invocations),
    _variantArchitectureInvocations(variantArchitectureInvocations)
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

    std::vector<pbxbuild::ToolInvocation> allInvocations;
    std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> variantArchitectureInvocations;

    pbxspec::PBX::Tool::shared_ptr scriptTool = buildEnvironment.specManager()->tool("com.apple.commands.shell-script", targetEnvironment.specDomain());

    // TODO(grp): This should probably try a number of other compilers if it's not clang.
    std::string gccVersion = targetEnvironment.environment().resolve("GCC_VERSION");
    pbxspec::PBX::Compiler::shared_ptr defaultCompiler = buildEnvironment.specManager()->compiler(gccVersion + ".compiler", targetEnvironment.specDomain());

    if (scriptTool == nullptr || defaultCompiler == nullptr) {
        fprintf(stderr, "error: couldn't get compiler tools\n");
        return nullptr;
    }

    std::string workingDirectory = targetEnvironment.workingDirectory();

    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment currentEnvironment = targetEnvironment.environment();
            currentEnvironment.insertFront(PhaseContext::VariantLevel(variant), false);
            currentEnvironment.insertFront(PhaseContext::ArchitectureLevel(arch), false);

            std::vector<pbxbuild::ToolInvocation> invocations;

            std::vector<pbxbuild::TypeResolvedFile> files = phaseContext.resolveBuildFiles(currentEnvironment, buildPhase->files());
            for (pbxbuild::TypeResolvedFile const &file : files) {
                pbxbuild::TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);

                if (buildRule != nullptr) {
                    if (buildRule->tool() != nullptr) {
                        pbxspec::PBX::Tool::shared_ptr tool = buildRule->tool();
                        if (tool->identifier() == "com.apple.compilers.gcc") {
                            auto context = pbxbuild::CompilerInvocationContext::Create(defaultCompiler, file, currentEnvironment, workingDirectory);
                            invocations.push_back(context.invocation());
                        } else {
                            // TODO(grp): Use an appropriate compiler context to create this invocation.
                            auto context = pbxbuild::ToolInvocationContext::Create(tool, { }, { file.filePath() }, currentEnvironment, workingDirectory);
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

    return std::make_unique<SourcesResolver>(allInvocations, variantArchitectureInvocations);
}
