/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/ClangResolver.h>
#include <pbxbuild/Tool/HeadermapResolver.h>
#include <pbxbuild/Tool/CompilerCommon.h>
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/Tool/CompilationInfo.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Tokens.h>
#include <pbxsetting/Environment.h>
#include <pbxsetting/Type.h>
#include <pbxsetting/Value.h>
#include <libutil/FSUtil.h>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::ClangResolver::
ClangResolver(pbxspec::PBX::Compiler::shared_ptr const &compiler) :
    _compiler(compiler)
{
}

Tool::ClangResolver::
~ClangResolver()
{
}

static bool
DialectIsCPlusPlus(ext::optional<std::string> const &dialect)
{
    return dialect && (dialect->size() > 2 && dialect->substr(dialect->size() - 2) == "++");
}

static void
AppendDialectFlags(std::vector<std::string> *args, ext::optional<std::string> const &dialect, std::string const &dialectSuffix = "")
{
    if (dialect) {
        args->push_back("-x");
        args->push_back(*dialect + dialectSuffix);
    }
}

static void
AppendPrefixHeaderFlags(std::vector<std::string> *args, std::string const &prefixHeader)
{
    args->push_back("-include");
    args->push_back(prefixHeader);
}

static void
AppendFrameworkPathFlags(std::vector<std::string> *args, pbxsetting::Environment const &environment, Tool::SearchPaths const &searchPaths)
{
    std::vector<std::string> specialFrameworkPaths = {
        environment.resolve("BUILT_PRODUCTS_DIR"),
    };
    Tool::CompilerCommon::AppendCompoundFlags(args, "-F", true, specialFrameworkPaths);
    Tool::CompilerCommon::AppendCompoundFlags(args, "-F", true, searchPaths.frameworkSearchPaths());
}

static void
AppendCustomFlags(std::vector<std::string> *args, pbxsetting::Environment const &environment, ext::optional<std::string> const &dialect)
{
    std::vector<std::string> flagSettings;
    flagSettings.push_back("WARNING_CFLAGS");
    flagSettings.push_back("OPTIMIZATION_CFLAGS");
    if (DialectIsCPlusPlus(dialect)) {
        flagSettings.push_back("OTHER_CPLUSPLUSFLAGS");
    } else {
        flagSettings.push_back("OTHER_CFLAGS");
    }
    flagSettings.push_back("OTHER_CFLAGS_" + environment.resolve("CURRENT_VARIANT"));
    flagSettings.push_back("PER_ARCH_CFLAGS_" + environment.resolve("CURRENT_ARCH"));

    for (std::string const &flagSetting : flagSettings) {
        std::vector<std::string> flags = pbxsetting::Type::ParseList(environment.resolve(flagSetting));
        args->insert(args->end(), flags.begin(), flags.end());
    }
}

static void
AppendNotUsedInPrecompsFlags(std::vector<std::string> *args, pbxsetting::Environment const &environment)
{
    std::vector<std::string> preprocessorDefinitions = pbxsetting::Type::ParseList(environment.resolve("GCC_PREPROCESSOR_DEFINITIONS_NOT_USED_IN_PRECOMPS"));
    Tool::CompilerCommon::AppendCompoundFlags(args, "-D", true, preprocessorDefinitions);

    std::vector<std::string> otherFlags = pbxsetting::Type::ParseList(environment.resolve("GCC_OTHER_CFLAGS_NOT_USED_IN_PRECOMPS"));
    args->insert(args->end(), otherFlags.begin(), otherFlags.end());
}

static void
AppendDependencyInfoFlags(std::vector<std::string> *args, pbxspec::PBX::Compiler::shared_ptr const &compiler, pbxsetting::Environment const &environment)
{
    if (compiler->dependencyInfoArgs()) {
        for (pbxsetting::Value const &arg : *compiler->dependencyInfoArgs()) {
            args->push_back(environment.expand(arg));
        }
    }
}

static void
AppendInputOutputFlags(std::vector<std::string> *args, pbxspec::PBX::Compiler::shared_ptr const &compiler, std::string const &input, std::string const &output)
{
    if (compiler->sourceFileOption()) {
        args->push_back(*compiler->sourceFileOption());
    } else {
        args->push_back("-c");
    }
    args->push_back(input);

    args->push_back("-o");
    args->push_back(output);
}

static std::string
CompileLogMessage(
    pbxspec::PBX::Compiler::shared_ptr const &compiler,
    std::string const &logTitle,
    std::string const &input,
    pbxspec::PBX::FileType::shared_ptr const &fileType,
    std::string const &output,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    std::string logMessage;
    logMessage += logTitle + " ";
    logMessage += output + " ";
    logMessage += FSUtil::GetRelativePath(input, workingDirectory) + " ";
    logMessage += environment.resolve("variant") + " ";
    logMessage += environment.resolve("arch") + " ";
    if (fileType->GCCDialectName()) {
        logMessage += *fileType->GCCDialectName() + " ";
    }
    logMessage += compiler->identifier();
    return logMessage;
}

void Tool::ClangResolver::
resolvePrecompiledHeader(
    Tool::Context *toolContext,
    pbxsetting::Environment const &environment,
    Tool::PrecompiledHeaderInfo const &precompiledHeaderInfo
) const
{
    std::string const &input = precompiledHeaderInfo.prefixHeader();
    pbxspec::PBX::FileType::shared_ptr const &fileType = precompiledHeaderInfo.fileType();
    std::string output = environment.expand(precompiledHeaderInfo.compileOutputPath());

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (_compiler);
    Tool::Environment toolEnvironment = Tool::Environment::Create(tool, environment, toolContext->workingDirectory(), { input }, { output });
    pbxsetting::Environment const &env = toolEnvironment.environment();
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), fileType);
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options);

    std::vector<std::string> arguments = precompiledHeaderInfo.arguments();
    AppendDependencyInfoFlags(&arguments, _compiler, env);
    AppendInputOutputFlags(&arguments, _compiler, input, output);

    ext::optional<std::string> const &dialect = fileType->GCCDialectName();
    std::string logTitle = DialectIsCPlusPlus(dialect) ? "ProcessPCH++" : "ProcessPCH";
    std::string logMessage = CompileLogMessage(_compiler, logTitle, input, fileType, output, env, toolContext->workingDirectory());

    auto serializedFile = Tool::Invocation::AuxiliaryFile::Data(
        env.expand(precompiledHeaderInfo.serializedOutputPath()),
        precompiledHeaderInfo.serialize());

    std::vector<Tool::Invocation::DependencyInfo> dependencyInfo;
    if (_compiler->dependencyInfoFile()) {
        dependencyInfo.push_back(Tool::Invocation::DependencyInfo(
            dependency::DependencyInfoFormat::Makefile,
            env.expand(*_compiler->dependencyInfoFile())));
    }

    Tool::Invocation invocation;
    invocation.executable() = Tool::Invocation::Executable::Determine(tokens.executable());
    invocation.arguments() = arguments;
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.dependencyInfo() = dependencyInfo;
    invocation.auxiliaryFiles().push_back(serializedFile);
    invocation.logMessage() = logMessage;
    toolContext->invocations().push_back(invocation);
}

void Tool::ClangResolver::
resolveSource(
    Tool::Context *toolContext,
    pbxsetting::Environment const &environment,
    Phase::File const &input,
    std::string const &outputDirectory) const
{
    Tool::HeadermapInfo const &headermapInfo = toolContext->headermapInfo();

    std::string resolvedOutputDirectory;
    if (_compiler->outputDir()) {
        resolvedOutputDirectory = environment.expand(*_compiler->outputDir());
    } else {
        resolvedOutputDirectory = outputDirectory;
    }

    std::string outputExtension = _compiler->outputFileExtension().value_or("o");

    std::string outputBaseName = FSUtil::GetBaseNameWithoutExtension(input.path());
    if (!input.fileNameDisambiguator().empty()) {
        outputBaseName = input.fileNameDisambiguator();
    }
    std::string output = resolvedOutputDirectory + "/" + outputBaseName + "." + outputExtension;

    pbxspec::PBX::FileType::shared_ptr const &fileType = input.fileType();
    std::vector<std::string> const &inputArguments = input.buildFile()->compilerFlags();

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (_compiler);
    Tool::Environment toolEnvironment = Tool::Environment::Create(tool, environment, toolContext->workingDirectory(), { input }, { output });
    pbxsetting::Environment const &env = toolEnvironment.environment();

    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), fileType);
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options);

    std::vector<std::string> inputDependencies;
    inputDependencies.insert(inputDependencies.end(), headermapInfo.systemHeadermapFiles().begin(), headermapInfo.systemHeadermapFiles().end());
    inputDependencies.insert(inputDependencies.end(), headermapInfo.userHeadermapFiles().begin(), headermapInfo.userHeadermapFiles().end());

    std::vector<std::string> arguments;
    AppendDialectFlags(&arguments, fileType->GCCDialectName());
    size_t dialectOffset = arguments.size();

    arguments.insert(arguments.end(), tokens.arguments().begin(), tokens.arguments().end());
    Tool::CompilerCommon::AppendIncludePathFlags(&arguments, env, toolContext->searchPaths(), headermapInfo);
    AppendFrameworkPathFlags(&arguments, env, toolContext->searchPaths());
    AppendCustomFlags(&arguments, env, fileType->GCCDialectName());

    bool precompilePrefixHeader = pbxsetting::Type::ParseBoolean(env.resolve("GCC_PRECOMPILE_PREFIX_HEADER"));
    std::string prefixHeader = env.resolve("GCC_PREFIX_HEADER");
    std::shared_ptr<Tool::PrecompiledHeaderInfo> precompiledHeaderInfo = nullptr;

    if (!prefixHeader.empty()) {
        std::string prefixHeaderFile = FSUtil::ResolveRelativePath(prefixHeader, toolContext->workingDirectory());

        if (precompilePrefixHeader) {
            std::vector<std::string> precompiledHeaderArguments;
            AppendDialectFlags(&precompiledHeaderArguments, fileType->GCCDialectName(), "-header");
            precompiledHeaderArguments.insert(precompiledHeaderArguments.end(), arguments.begin() + dialectOffset, arguments.end());
            // Added below, but need to have here in case it affects the precompiled header (as it often does).
            precompiledHeaderArguments.insert(precompiledHeaderArguments.end(), inputArguments.begin(), inputArguments.end());

            precompiledHeaderInfo = std::make_shared<Tool::PrecompiledHeaderInfo>(PrecompiledHeaderInfo::Create(_compiler, prefixHeaderFile, fileType, precompiledHeaderArguments));
            AppendPrefixHeaderFlags(&arguments, env.expand(precompiledHeaderInfo->logicalOutputPath()));
            inputDependencies.push_back(env.expand(precompiledHeaderInfo->compileOutputPath()));
        } else {
            AppendPrefixHeaderFlags(&arguments, prefixHeaderFile);
            inputDependencies.push_back(prefixHeaderFile);
        }
    }

    AppendNotUsedInPrecompsFlags(&arguments, env);
    // After all of the configurable settings, so they can override.
    arguments.insert(arguments.end(), inputArguments.begin(), inputArguments.end());
    AppendDependencyInfoFlags(&arguments, _compiler, env);
    AppendInputOutputFlags(&arguments, _compiler, input.path(), output);

    std::string logMessage = CompileLogMessage(_compiler, "CompileC", input.path(), fileType, output, env, toolContext->workingDirectory());

    std::vector<Tool::Invocation::DependencyInfo> dependencyInfo;
    if (_compiler->dependencyInfoFile()) {
        dependencyInfo.push_back(Tool::Invocation::DependencyInfo(
            dependency::DependencyInfoFormat::Makefile,
            env.expand(*_compiler->dependencyInfoFile())));
    }

    Tool::Invocation invocation;
    invocation.executable() = Tool::Invocation::Executable::Determine(tokens.executable());
    invocation.arguments() = arguments;
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.inputDependencies() = inputDependencies;
    invocation.dependencyInfo() = dependencyInfo;
    invocation.logMessage() = logMessage;

    /* Add the compilation invocation to the context. */
    toolContext->invocations().push_back(invocation);
    auto variantArchitectureKey = std::make_pair(environment.resolve("variant"), environment.resolve("arch"));
    toolContext->variantArchitectureInvocations()[variantArchitectureKey].push_back(invocation);

    Tool::CompilationInfo *compilationInfo = &toolContext->compilationInfo();

    /* If we have precompiled header info, create an invocation for the precompiled header. */
    if (precompiledHeaderInfo != nullptr) {
        std::string hash = precompiledHeaderInfo->hash();

        auto precompiledHeaderInfoMap = &compilationInfo->precompiledHeaderInfo();
        if (precompiledHeaderInfoMap->find(hash) == precompiledHeaderInfoMap->end()) {
            /* This precompiled header wasn't already created, create it now. */
            precompiledHeaderInfoMap->insert({ hash, *precompiledHeaderInfo });

            resolvePrecompiledHeader(
                toolContext,
                environment,
                *precompiledHeaderInfo
            );
        }
    }

    if (DialectIsCPlusPlus(fileType->GCCDialectName()) && _compiler->execCPlusPlusLinkerPath()) {
        /* If a single C++ file is seen, use the C++ linker driver. */
        compilationInfo->linkerDriver() = *_compiler->execCPlusPlusLinkerPath();
    } else if (compilationInfo->linkerDriver().empty() && _compiler->execPath()) {
        /* If a C file is seen after a C++ file, don't reset back to the C driver. */
        compilationInfo->linkerDriver() = _compiler->execPath()->raw();
    }

    for (std::string const &linkerArg : options.linkerArgs()) {
        std::vector<std::string> *linkerArguments = &compilationInfo->linkerArguments();

        /* Avoid duplicating arguments for multiple compiler invocations. */
        if (std::find(linkerArguments->begin(), linkerArguments->end(), linkerArg) == linkerArguments->end()) {
            linkerArguments->push_back(linkerArg);
        }
    }
}

std::unique_ptr<Tool::ClangResolver> Tool::ClangResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    // TODO(grp): This should probably try a number of other compilers if it's not clang.
    std::string gccVersion = targetEnvironment.environment().resolve("GCC_VERSION");

    // TODO(grp): Depending on the build action, add a different suffix than ".compiler".
    pbxspec::PBX::Compiler::shared_ptr defaultCompiler = buildEnvironment.specManager()->compiler(gccVersion + ".compiler", targetEnvironment.specDomains());
    if (defaultCompiler == nullptr) {
        // TODO(grp): Should this fallback to a hardcoded default compiler here?
        defaultCompiler = buildEnvironment.specManager()->compiler("com.apple.compilers.llvm.clang.1_0.compiler", targetEnvironment.specDomains());
        if (defaultCompiler == nullptr) {
            fprintf(stderr, "error: couldn't get default compiler\n");
            return nullptr;
        }
    }

    return std::unique_ptr<Tool::ClangResolver>(new Tool::ClangResolver(defaultCompiler));
}
