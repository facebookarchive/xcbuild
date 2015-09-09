// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/LinkerInvocationContext.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::LinkerInvocationContext;
using pbxbuild::ToolInvocationContext;
using ToolEnvironment = pbxbuild::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::ToolInvocationContext::CommandLineResult;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

LinkerInvocationContext::
LinkerInvocationContext(ToolInvocation const &invocation) :
    _invocation(invocation)
{
}

LinkerInvocationContext::
~LinkerInvocationContext()
{
}

LinkerInvocationContext LinkerInvocationContext::
Create(
    pbxspec::PBX::Linker::shared_ptr const &linker,
    std::vector<std::string> const &inputFiles,
    std::vector<TypeResolvedFile> const &inputLibraries,
    std::string const &output,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &executable
)
{
    std::vector<std::string> special;
    std::vector<ToolInvocation::AuxiliaryFile> auxiliaries;

    if (linker->supportsInputFileList()) {
        std::string path = environment.expand(pbxsetting::Value::Parse("$(LINK_FILE_LIST_$(variant)_$(arch))"));

        std::string contents;
        for (std::string const &input : inputFiles) {
            contents += input + "\n";
        }

        ToolInvocation::AuxiliaryFile fileList = ToolInvocation::AuxiliaryFile(path, contents, false);
        auxiliaries.push_back(fileList);

        special.push_back("-filelist");
        special.push_back(path);
    }

    for (TypeResolvedFile const &library : inputLibraries) {
        std::string base = FSUtil::GetBaseNameWithoutExtension(library.filePath());
        if (library.fileType()->isFrameworkWrapper()) {
            special.push_back("-framework");
            special.push_back(base);
        } else {
            if (base.find("lib") == 0) {
                base = base.substr(3);
            }
            special.push_back("-l" + base);
        }
    }

    // TODO(grp): Do something more with this. Set $(DependencyInfoFile) in the environment?
    std::string dependencyInfo = linker->dependencyInfoFile();

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (linker);
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputFiles, { output });
    OptionsResult options = OptionsResult::Create(toolEnvironment, nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, executable, special);
    std::string logMessage = ToolInvocationContext::LogMessage(toolEnvironment);
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory, dependencyInfo, auxiliaries);
    return LinkerInvocationContext(context.invocation());
}
