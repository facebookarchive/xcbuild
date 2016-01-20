/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/SwiftResolver.h>
#include <pbxbuild/Tool/CompilerCommon.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Tokens.h>
#include <pbxbuild/Tool/Context.h>

namespace Tool = pbxbuild::Tool;
namespace Phase = pbxbuild::Phase;
using libutil::FSUtil;

Tool::SwiftResolver::
SwiftResolver(pbxspec::PBX::Compiler::shared_ptr const &compiler) :
    _compiler(compiler)
{
}

static std::string
SwiftLibraryPath(pbxsetting::Environment const &environment)
{
    std::string path = environment.resolve("SWIFT_LIBRARY_PATH");
    if (path.empty()) {
        // TODO(grp): Get the path by finding the Swift libraries in the toolchain.
    }
    return path;
}

static void
AppendOutputs(
    std::vector<std::string> *args,
    std::vector<std::string> *outputs,
    std::vector<Tool::Invocation::DependencyInfo> *dependencyInfo,
    std::vector<Tool::Invocation::AuxiliaryFile> *auxiliaryFiles,
    std::string const &outputDirectory,
    std::string const &moduleName,
    std::string const &modulePath,
    std::vector<Phase::File> const &inputs)
{
    std::unique_ptr<plist::Dictionary> outputInfo = plist::Dictionary::New();

    /* Top-level outputs for the entire module. */
    std::string moduleDependencies = outputDirectory + "/" + moduleName + "-master.swiftdeps";

    std::unique_ptr<plist::Dictionary> module = plist::Dictionary::New();
    module->set("swift-dependencies", plist::String::New(moduleDependencies));
    outputInfo->set("", std::move(module));

    for (Phase::File const &input : inputs) {
        std::string name = FSUtil::GetBaseNameWithoutExtension(input.path());

        /* Add input argument. */
        args->push_back(input.path());

        /* All of the outputs for each file. */
        std::string swiftModule       = outputDirectory + "/" + name + "~partial.swiftmodule";
        std::string object            = outputDirectory + "/" + name + ".o";
        std::string llvmbc            = outputDirectory + "/" + name + ".bc";
        std::string diagnostics       = outputDirectory + "/" + name + ".dia";
        std::string dependencies      = outputDirectory + "/" + name + ".d";
        std::string swiftDependencies = outputDirectory + "/" + name + ".swiftdeps";

        /* All are outputs of the Swift invocation. */
        outputs->push_back(object);
        outputs->push_back(swiftModule);
        outputs->push_back(llvmbc);
        outputs->push_back(diagnostics);
        outputs->push_back(dependencies);
        outputs->push_back(swiftDependencies);

        /* Add dependency info for output. */
        auto info = Tool::Invocation::DependencyInfo(dependency::DependencyInfoFormat::Makefile, dependencies);
        dependencyInfo->push_back(info);

        /* Create output map for this file */
        std::unique_ptr<plist::Dictionary> dict = plist::Dictionary::New();
        dict->set("swiftmodule", plist::String::New(swiftModule));
        dict->set("object", plist::String::New(object));
        dict->set("llvm-bc", plist::String::New(llvmbc));
        dict->set("diagnostics", plist::String::New(diagnostics));
        dict->set("dependencies", plist::String::New(dependencies));
        dict->set("swift-dependencies", plist::String::New(swiftDependencies));
        outputInfo->set(input.path(), std::move(dict));
    }

    /* Serialize output map as JSON. */
    auto serialized = plist::Format::JSON::Serialize(outputInfo.get(), plist::Format::JSON::Create());
    if (!serialized.first) {
        fprintf(stderr, "error: %s\n", serialized.second.c_str());
    } else {
        /* Write output map as an auxiliary file. */
        std::string outputInfoPath = outputDirectory + "/" + moduleName + "-OutputFileMap.json";
        Tool::Invocation::AuxiliaryFile outputInfoFile = Tool::Invocation::AuxiliaryFile(outputInfoPath, *serialized.first, false);
        auxiliaryFiles->push_back(outputInfoFile);

        /* Add output info to the arguments. */
        args->push_back("-output-file-map");
        args->push_back(outputInfoPath);
    }

    /* Tell Swift to output various files. Paths are provided in the output map. */
    args->push_back("-parseable-output");
    args->push_back("-serialize-diagnostics");
    args->push_back("-emit-dependencies");

    /* Emit the resulting module. */
    args->push_back("-emit-module");
    args->push_back("-emit-module-path");
    args->push_back(modulePath);

    // TODO(grp): Copy the module to BUILT_PRODUCTS_DIR.
}

static void
AppendPathFlags(std::vector<std::string> *args, pbxsetting::Environment const &environment)
{
    /* Note: Swift paths are passed un-concatenated as two arguments. */

    std::vector<std::string> specialIncludes = { environment.resolve("BUILT_PRODUCTS_DIR") };
    Tool::CompilerCommon::AppendCompoundFlags(args, "-I", false, specialIncludes);

    std::vector<std::string> includes = pbxsetting::Type::ParseList(environment.resolve("SWIFT_INCLUDE_PATHS"));
    Tool::CompilerCommon::AppendCompoundFlags(args, "-I", false, includes);

    std::vector<std::string> frameworks = pbxsetting::Type::ParseList(environment.resolve("FRAMEWORK_SEARCH_PATHS"));
    Tool::CompilerCommon::AppendCompoundFlags(args, "-F", false, frameworks);
}

static void
AppendCcFlags(std::vector<std::string> *args, pbxsetting::Environment const &environment, Tool::SearchPaths const &searchPaths, Tool::HeadermapInfo const &headermapInfo)
{
    std::vector<std::string> arguments;

    /* Add include paths. */
    Tool::CompilerCommon::AppendIncludePathFlags(&arguments, environment, searchPaths, headermapInfo);

    /* Add definitions. */
    std::vector<std::string> definitions = pbxsetting::Type::ParseList(environment.resolve("GCC_PREPROCESSOR_DEFINITIONS"));
    Tool::CompilerCommon::AppendCompoundFlags(&arguments, "-D", true, definitions);

    /* Add flags prefixed with -Xcc. */
    for (std::string const &arg : arguments) {
        args->push_back("-Xcc");
        args->push_back(arg);
    }
}

static void
AppendObjcHeader(std::vector<std::string> *args, std::vector<std::string> *outputs, pbxsetting::Environment const &environment, std::string const &outputDirectory)
{
    /* Output the generated header. */
    std::string headerName = environment.resolve("SWIFT_OBJC_INTERFACE_HEADER_NAME");
    std::string headerPath = outputDirectory + "/" + headerName;
    outputs->push_back(headerPath);

    args->push_back("-emit-objc-header");
    args->push_back("-emit-objc-header-path");
    args->push_back(headerPath);

    /* Copy the generated header, if requested. */
    if (pbxsetting::Type::ParseBoolean(environment.resolve("SWIFT_INSTALL_OBJC_HEADER"))) {
        std::string installedPath = environment.resolve("TARGET_BUILD_DIR") + "/" + environment.resolve("PUBLIC_HEADERS_FOLDER_PATH") + "/" + headerName;
        // TODO(grp): Copy the heaader to installedPath.
    } else {
        // TODO(grp): Copy the heaader to DerivedSources.
    }

    /* Load the bridging header. */
    std::string bridgingHeader = environment.resolve("SWIFT_OBJC_BRIDGING_HEADER");
    if (!bridgingHeader.empty()) {
        args->push_back("-import-objc-header");
        args->push_back(bridgingHeader);
    }
}

void Tool::SwiftResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<Phase::File> const &inputs,
    std::string const &outputDirectory) const
{
    /*
     * Resolve the tool options.
     */
    Tool::Environment toolEnvironment = Tool::Environment::Create(_compiler, baseEnvironment, toolContext->workingDirectory(), inputs);
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options);

    pbxsetting::Environment const &environment = toolEnvironment.environment();

    /*
     * Empty result appended to below.
     */
    std::vector<std::string> outputs;
    std::vector<Tool::Invocation::DependencyInfo> dependencyInfo;
    std::vector<Tool::Invocation::AuxiliaryFile> auxiliaryFiles;
    std::vector<std::string> arguments = tokens.arguments();

    /*
     * Append standard Swift flags.
     */
    AppendPathFlags(&arguments, environment);

    /* Compile as a library if no main.swift. */
    bool hasMain = false;
    for (Phase::File const &input : inputs) {
        if (FSUtil::GetBaseName(input.path()) == "main.swift") {
            hasMain = true;
            break;
        }
    }
    if (!hasMain) {
        /* The specification will already pass this flag if SWIFT_LIBRARIES_ONLY is YES. */
        if (!pbxsetting::Type::ParseBoolean(environment.resolve("SWIFT_LIBRARIES_ONLY"))) {
            arguments.push_back("-parse-as-library");
        }
    }

    /* Compile object files. */
    arguments.push_back("-c");

    /* Enable parallelization. */
    bool wholeModuleOptimization = (pbxsetting::Type::ParseBoolean(environment.resolve("SWIFT_WHOLE_MODULE_OPTIMIZATION")) || environment.resolve("SWIFT_OPTIMIZATION_LEVEL") == "-Owholemodule");
    if (!wholeModuleOptimization || !pbxsetting::Type::ParseBoolean(environment.resolve("SWIFT_USE_PARALLEL_WHOLE_MODULE_OPTIMIZATION"))) {
        // TODO(grp): Get the number of parallel build tasks here.
        arguments.push_back("-j8");
    } else {
        // TODO(grp): Get the number of parallel build tasks here.
        arguments.push_back("-num-threads");
        arguments.push_back("8");
    }

    /*
     * Add inputs and outputs to the invocation.
     */
    std::string moduleName = environment.resolve("SWIFT_MODULE_NAME");
    std::string modulePath = outputDirectory + "/" + moduleName + ".swiftmodule";
    AppendOutputs(&arguments, &outputs, &dependencyInfo, &auxiliaryFiles, outputDirectory, moduleName, modulePath, inputs);

    /*
     * Add flags for interacting with Objective-C code.
     */
    AppendCcFlags(&arguments, environment, toolContext->searchPaths(), toolContext->headermapInfo());
    AppendObjcHeader(&arguments, &outputs, environment, outputDirectory);

    /* Compiler working directory. */
    arguments.push_back("-Xcc");
    arguments.push_back("-working-directory" + toolContext->workingDirectory());

    /* Log message for the outer compile step. */
    std::string logMessage = "CompileSwiftSources " + environment.resolve("variant") + " " + environment.resolve("arch") + " " + _compiler->identifier();

    Tool::Invocation invocation;
    invocation.executable() = tokens.executable();
    invocation.arguments() = arguments;
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = outputs;
    invocation.dependencyInfo() = dependencyInfo;
    invocation.auxiliaryFiles() = auxiliaryFiles;
    invocation.logMessage() = logMessage;
    toolContext->invocations().push_back(invocation);

    auto variantArchitectureKey = std::make_pair(environment.resolve("variant"), environment.resolve("arch"));
    toolContext->variantArchitectureInvocations()[variantArchitectureKey].push_back(invocation);

    Tool::CompilationInfo *compilationInfo = &toolContext->compilationInfo();

    /* Default to Clang as a linker. */
    if (compilationInfo->linkerDriver().empty()) {
        compilationInfo->linkerDriver() = "clang";
    }

    // TODO(grp): For multi-arch builds the below flags get added twice.

    /* Add Swift libraries to linker arguments. */
    compilationInfo->linkerArguments().push_back("-L" + SwiftLibraryPath(environment));

    /* Add Swift module to the linked result to allow debugging. */
    if (pbxsetting::Type::ParseBoolean(environment.resolve("GCC_GENERATE_DEBUGGING_SYMBOLS"))) {
        compilationInfo->linkerArguments().push_back("-Xlinker");
        compilationInfo->linkerArguments().push_back("-add_ast_path");
        compilationInfo->linkerArguments().push_back("-Xlinker");
        compilationInfo->linkerArguments().push_back(modulePath);
    }
}

std::unique_ptr<Tool::SwiftResolver> Tool::SwiftResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Compiler::shared_ptr swiftTool = buildEnvironment.specManager()->compiler(Tool::SwiftResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (swiftTool == nullptr) {
        fprintf(stderr, "warning: could not find asset catalog compiler\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::SwiftResolver>(new Tool::SwiftResolver(swiftTool));
}

