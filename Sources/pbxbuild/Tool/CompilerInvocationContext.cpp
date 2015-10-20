// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Tool/CompilerInvocationContext.h>
#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::CompilerInvocationContext;
using pbxbuild::Tool::ToolInvocationContext;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::Tool::HeadermapInvocationContext;
using pbxbuild::Tool::SearchPaths;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

CompilerInvocationContext::
CompilerInvocationContext(ToolInvocation const &invocation, std::string const &output, std::vector<std::string> const &linkerArgs) :
    _invocation(invocation),
    _output    (output),
    _linkerArgs(linkerArgs)
{
}

CompilerInvocationContext::
~CompilerInvocationContext()
{
}

static void
AppendPathFlag(std::vector<std::string> *args, std::string const &path, std::string const &prefix, bool concatenate)
{
    if (concatenate) {
        args->push_back(prefix + path);
    } else {
        args->push_back(prefix);
        args->push_back(path);
    }
}

static void
AppendPathFlags(std::vector<std::string> *args, std::vector<std::string> const &paths, std::string const &prefix, bool concatenate)
{
    for (std::string const &path : paths) {
        AppendPathFlag(args, path, prefix, concatenate);
    }
}

CompilerInvocationContext CompilerInvocationContext::
CreateSource(
    pbxspec::PBX::Compiler::shared_ptr const &compiler,
    TypeResolvedFile const &input,
    std::vector<std::string> const &inputArguments,
    std::string const &outputBaseName,
    std::string const &prefixHeader,
    HeadermapInvocationContext const &headermaps,
    SearchPaths const &searchPaths,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    std::string outputDirectory = environment.expand(compiler->outputDir());
    if (outputDirectory.empty()) {
        outputDirectory = environment.expand(pbxsetting::Value::Parse("$(OBJECT_FILE_DIR_$(variant))/$(arch)"));
    }

    std::string outputExtension = compiler->outputFileExtension();
    if (outputExtension.empty()) {
        outputExtension = "o";
    }

    std::string output = outputDirectory + "/" + outputBaseName + "." + outputExtension;

    return Create(
        compiler,
        input.filePath(),
        input.fileType(),
        std::string(),
        inputArguments,
        output,
        prefixHeader,
        headermaps,
        searchPaths,
        environment,
        workingDirectory,
        "CompileC"
    );
}

CompilerInvocationContext CompilerInvocationContext::
CreatePrecompiledHeader(
    pbxspec::PBX::Compiler::shared_ptr const &compiler,
    std::string const &input,
    pbxspec::PBX::FileType::shared_ptr const &fileType,
    HeadermapInvocationContext const &headermaps,
    SearchPaths const &searchPaths,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    std::string const &dialect = fileType->GCCDialectName();
    bool plusplus = (dialect.size() > 2 && dialect.substr(dialect.size() - 2) == "++");
    std::string logTitle = plusplus ? "ProcessPCH++" : "ProcessPCH";

    // TODO(grp): This path generation should use a hash, not just the dialect.
    std::string outputDirectory = environment.resolve("PRECOMP_DESTINATION_DIR") + "/" + environment.resolve("PRODUCT_NAME") + "-" + fileType->GCCDialectName();
    std::string output = outputDirectory + "/" + FSUtil::GetBaseName(input) + ".pch";

    return Create(
        compiler,
        input,
        fileType,
        "-header",
        std::vector<std::string>(),
        output,
        std::string(),
        headermaps,
        searchPaths,
        environment,
        workingDirectory,
        logTitle
    );
}

CompilerInvocationContext CompilerInvocationContext::
Create(
    pbxspec::PBX::Compiler::shared_ptr const &compiler,
    std::string const &input,
    pbxspec::PBX::FileType::shared_ptr const &fileType,
    std::string const &dialectSuffix,
    std::vector<std::string> const &inputArguments,
    std::string const &output,
    std::string const &prefixHeader,
    HeadermapInvocationContext const &headermaps,
    SearchPaths const &searchPaths,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &logTitle
)
{
    std::vector<std::string> inputFiles;
    inputFiles.push_back(input);
    inputFiles.insert(inputFiles.end(), headermaps.systemHeadermapFiles().begin(), headermaps.systemHeadermapFiles().end());
    inputFiles.insert(inputFiles.end(), headermaps.userHeadermapFiles().begin(), headermaps.userHeadermapFiles().end());
    if (!prefixHeader.empty()) {
        inputFiles.push_back(prefixHeader);
    }

    std::vector<std::string> outputFiles;
    outputFiles.push_back(output);

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (compiler);
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputFiles, outputFiles);
    pbxsetting::Environment const &env = toolEnvironment.toolEnvironment();

    std::vector<std::string> special;

    std::string const &dialect = fileType->GCCDialectName();
    special.push_back("-x");
    special.push_back(dialect + dialectSuffix);

    std::vector<std::string> flagSettings;
    if (dialect.size() > 2 && dialect.substr(dialect.size() - 2) == "++") {
        flagSettings.push_back("OTHER_CPLUSPLUSFLAGS");
    } else {
        flagSettings.push_back("OTHER_CFLAGS");
    }
    flagSettings.push_back("OTHER_CFLAGS_" + env.resolve("CURRENT_VARIANT"));
    flagSettings.push_back("PER_ARCH_CFLAGS_" + env.resolve("CURRENT_ARCH"));
    flagSettings.push_back("WARNING_CFLAGS");
    flagSettings.push_back("OPTIMIZATION_CFLAGS");

    for (std::string const &flagSetting : flagSettings) {
        std::vector<std::string> flags = pbxsetting::Type::ParseList(env.resolve(flagSetting));
        special.insert(special.end(), flags.begin(), flags.end());
    }

    AppendPathFlags(&special, headermaps.systemHeadermapFiles(), "-I", true);
    AppendPathFlags(&special, headermaps.userHeadermapFiles(), "-iquote", false);

    if (env.resolve("USE_HEADER_SYMLINKS") == "YES") {
        // TODO(grp): Create this symlink tree as needed.
		AppendPathFlags(&special, { env.resolve("CPP_HEADER_SYMLINKS_DIR") }, "-I", true);
    }

    std::vector<std::string> specialIncludePaths = {
        env.resolve("DERIVED_FILE_DIR"),
        env.resolve("DERIVED_FILE_DIR") + "/" + env.resolve("arch"),
        env.resolve("BUILT_PRODUCTS_DIR") + "/include",
    };
    AppendPathFlags(&special, specialIncludePaths, "-I", true);
    AppendPathFlags(&special, searchPaths.userHeaderSearchPaths(), "-I", true);
    AppendPathFlags(&special, searchPaths.headerSearchPaths(), "-I", true);

    std::vector<std::string> specialFrameworkPaths = {
        env.resolve("BUILT_PRODUCTS_DIR"),
    };
    AppendPathFlags(&special, specialFrameworkPaths, "-F", true);
    AppendPathFlags(&special, searchPaths.frameworkSearchPaths(), "-F", true);

    if (!prefixHeader.empty()) {
        special.push_back("-include");
        if (prefixHeader.size() > 8 && prefixHeader.substr(prefixHeader.size() - 8) == ".pch.pch") {
            special.push_back(prefixHeader.substr(0, prefixHeader.size() - 4));
        } else {
            special.push_back(prefixHeader);
        }
    }

    // After all of the configurable settings, so they can override.
    special.insert(special.end(), inputArguments.begin(), inputArguments.end());

    std::string sourceFileOption = compiler->sourceFileOption();
    if (sourceFileOption.empty()) {
        sourceFileOption = "-c";
    }
    special.push_back(sourceFileOption);
    special.push_back(input);

    special.push_back("-o");
    special.push_back(output);

    std::string logMessage = logTitle +" " + output + " " + FSUtil::GetRelativePath(input, workingDirectory) + " " + environment.resolve("variant") + " " + environment.resolve("arch") + " " + dialect + " " + compiler->identifier();

    OptionsResult options = OptionsResult::Create(toolEnvironment, workingDirectory, fileType);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "", special);
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory);
    return CompilerInvocationContext(context.invocation(), output, options.linkerArgs());
}
