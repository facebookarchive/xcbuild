// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/CompilerInvocationContext.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::CompilerInvocationContext;
using pbxbuild::ToolInvocationContext;
using ToolEnvironment = pbxbuild::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::ToolInvocationContext::CommandLineResult;
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
AppendPathFlags(std::vector<std::string> *args, std::string const &sdkroot, std::vector<std::string> const &paths, std::string const &prefix, bool concatenate)
{
    for (std::string path : paths) {
        if (FSUtil::IsAbsolutePath(path) && path.find(sdkroot) != 0) {
            path = sdkroot + path;
        }

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
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    std::vector<std::string> inputFiles;
    inputFiles.push_back(input.filePath());

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
    special.push_back(input.fileType()->GCCDialectName());

    std::string sdkroot = env.resolve("SDKROOT");

    if (env.resolve("USE_HEADERMAP") == "YES") {
        if (env.resolve("HEADERMAP_USES_VFS") == "YES") {
            // TODO(grp): Support VFS-based header maps.
            fprintf(stderr, "warning: VFS-based header maps not supported\n");
        }

        // TODO(grp): Create this header maps if needed.
        std::vector<std::string> headermapSystemPaths;
        if (env.resolve("ALWAYS_USE_SEPARATE_HEADERMAPS") == "YES") {
            headermapSystemPaths.push_back(env.resolve("CPP_HEADERMAP_FILE"));
        }
        headermapSystemPaths.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_OWN_TARGET_HEADERS"));
        headermapSystemPaths.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_ALL_TARGET_HEADERS"));
        if (false) {
            // TODO(grp): When should this be used?
            headermapSystemPaths.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_ALL_NON_FRAMEWORK_TARGET_HEADERS"));
        }
        AppendPathFlags(&special, sdkroot, headermapSystemPaths, "-I", true);

        std::vector<std::string> headermapUserPaths;
        headermapUserPaths.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_GENERATED_FILES"));
        headermapUserPaths.push_back(env.resolve("CPP_HEADERMAP_FILE_FOR_PROJECT_FILES"));
        AppendPathFlags(&special, sdkroot, headermapUserPaths, "-iquote", false);
    } else if (env.resolve("USE_HEADER_SYMLINKS") == "YES") {
        // TODO(grp): Create this symlink tree as needed.
		AppendPathFlags(&special, sdkroot, { env.resolve("CPP_HEADER_SYMLINKS_DIR") }, "-I", true);
    }

    AppendPathFlags(&special, sdkroot, environment.resolveList("USER_HEADER_SEARCH_PATHS"), "-iquote", false);
    AppendPathFlags(&special, sdkroot, environment.resolveList("HEADER_SEARCH_PATHS"), "-I", true);
    AppendPathFlags(&special, sdkroot, environment.resolveList("FRAMEWORK_SEARCH_PATHS"), "-F", true);

    std::vector<std::string> specialIncludePaths = {
        environment.resolve("DERIVED_FILE_DIR"),
        environment.resolve("DERIVED_FILE_DIR") + "/" + environment.resolve("arch"),
        environment.resolve("BUILT_PRODUCTS_DIR") + "/include",
    };
    AppendPathFlags(&special, sdkroot, specialIncludePaths, "-I", true);

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
