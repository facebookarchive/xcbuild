// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/CompilerInvocationContext.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/Tool/HeadermapInvocationContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::CompilerInvocationContext;
using pbxbuild::ToolInvocationContext;
using ToolEnvironment = pbxbuild::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::ToolInvocationContext::CommandLineResult;
using pbxbuild::Tool::HeadermapInvocationContext;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

CompilerInvocationContext::
CompilerInvocationContext(ToolInvocation const &invocation) :
    _invocation(invocation)
{
}

CompilerInvocationContext::
~CompilerInvocationContext()
{
}

static void
AppendPathFlags(std::vector<std::string> *args, std::vector<std::string> const &paths, std::string const &prefix, bool concatenate)
{
    for (std::string path : paths) {
        // TODO(grp): Apply trailing ** as a recursive search path.

        if (concatenate) {
            args->push_back(prefix + path);
        } else {
            args->push_back(prefix);
            args->push_back(path);
        }
    }
}

CompilerInvocationContext CompilerInvocationContext::
Create(
    pbxspec::PBX::Compiler::shared_ptr const &compiler,
    TypeResolvedFile const &input,
    std::vector<std::string> const &inputArguments,
    HeadermapInvocationContext const &headermaps,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    std::string const &dialect = input.fileType()->GCCDialectName();

    std::vector<std::string> inputFiles;
    inputFiles.push_back(input.filePath());
    inputFiles.insert(inputFiles.end(), headermaps.systemHeadermapFiles().begin(), headermaps.systemHeadermapFiles().end());
    inputFiles.insert(inputFiles.end(), headermaps.userHeadermapFiles().begin(), headermaps.userHeadermapFiles().end());

    std::vector<std::string> outputFiles;

    std::string outputDirectory = environment.expand(compiler->outputDir());
    if (outputDirectory.empty()) {
        outputDirectory = environment.expand(pbxsetting::Value::Parse("$(OBJECT_FILE_DIR_$(variant))/$(arch)"));
    }

    std::string outputExtension = compiler->outputFileExtension();
    if (outputExtension.empty()) {
        outputExtension = "o";
    }

    std::string output = environment.expand(pbxsetting::Value::Parse(outputDirectory)) + "/" + FSUtil::GetBaseNameWithoutExtension(input.filePath()) + "." + outputExtension;
    outputFiles.push_back(output);

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (compiler);
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputFiles, outputFiles);
    pbxsetting::Environment const &env = toolEnvironment.toolEnvironment();

    std::vector<std::string> special;
    special.push_back("-x");
    special.push_back(dialect);

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

    AppendPathFlags(&special, pbxsetting::Type::ParseList(env.resolve("USER_HEADER_SEARCH_PATHS")), "-iquote", false);
    AppendPathFlags(&special, pbxsetting::Type::ParseList(env.resolve("HEADER_SEARCH_PATHS")), "-I", true);
    AppendPathFlags(&special, pbxsetting::Type::ParseList(env.resolve("FRAMEWORK_SEARCH_PATHS")), "-F", true);

    std::vector<std::string> specialIncludePaths = {
        env.resolve("DERIVED_FILE_DIR"),
        env.resolve("DERIVED_FILE_DIR") + "/" + env.resolve("arch"),
        env.resolve("BUILT_PRODUCTS_DIR") + "/include",
    };
    AppendPathFlags(&special, specialIncludePaths, "-I", true);

    // TODO(grp): Use the precompiled prefix header, if/when precompilation is supported.
    std::string prefixHeader = env.resolve("GCC_PREFIX_HEADER");
    if (!prefixHeader.empty()) {
        special.push_back("-include");
        special.push_back(prefixHeader);
    }

    // After all of the configurable settings, so they can override.
    special.insert(special.end(), inputArguments.begin(), inputArguments.end());

    std::string sourceFileOption = compiler->sourceFileOption();
    if (sourceFileOption.empty()) {
        sourceFileOption = "-c";
    }
    special.push_back(sourceFileOption);
    special.push_back(input.filePath());

    special.push_back("-o");
    special.push_back(output);

    std::string logMessage = "CompileC " + output + " " + FSUtil::GetRelativePath(input.filePath(), workingDirectory) + " " + environment.resolve("variant") + " " + environment.resolve("arch") + " " + input.fileType()->GCCDialectName() + " " + compiler->identifier();

    OptionsResult options = OptionsResult::Create(toolEnvironment, input.fileType());
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "", special);
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory);
    return CompilerInvocationContext(context.invocation());
}
