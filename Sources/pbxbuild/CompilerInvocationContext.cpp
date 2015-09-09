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


    std::vector<std::string> special;
    special.push_back("-x");
    special.push_back(input.fileType()->GCCDialectName());

    std::string sourceFileOption = compiler->sourceFileOption();
    if (sourceFileOption.empty()) {
        sourceFileOption = "-c";
    }
    special.push_back(sourceFileOption);
    special.push_back(input.filePath());

    special.push_back("-o");
    special.push_back(output);

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (compiler);
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputFiles, outputFiles);

    std::string logMessage = "CompileC " + output + " " + FSUtil::GetRelativePath(input.filePath(), workingDirectory) + " " + environment.resolve("variant") + " " + environment.resolve("arch") + " " + input.fileType()->GCCDialectName() + " " + compiler->identifier();

    OptionsResult options = OptionsResult::Create(toolEnvironment, input.fileType());
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "", special);
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory);
    return CompilerInvocationContext(context.invocation());
}
