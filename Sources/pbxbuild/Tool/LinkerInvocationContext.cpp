// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Tool/LinkerInvocationContext.h>
#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::LinkerInvocationContext;
using pbxbuild::Tool::ToolInvocationContext;
using ToolEnvironment = pbxbuild::Tool::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::Tool::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::Tool::ToolInvocationContext::CommandLineResult;
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
    std::vector<std::string> const &additionalArguments,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &executable
)
{
    std::vector<std::string> special;
    std::vector<ToolInvocation::AuxiliaryFile> auxiliaries;

    special.insert(special.end(), additionalArguments.begin(), additionalArguments.end());

    if (linker->supportsInputFileList() || linker->identifier() == "com.apple.pbx.linkers.libtool") {
        std::string path = environment.expand(pbxsetting::Value::Parse("$(LINK_FILE_LIST_$(variant)_$(arch))"));
        std::string contents;
        for (std::string const &input : inputFiles) {
            contents += input + "\n";
        }
        ToolInvocation::AuxiliaryFile fileList = ToolInvocation::AuxiliaryFile(path, contents, false);
        auxiliaries.push_back(fileList);
    }

    std::unordered_set<std::string> libraryPaths;
    std::unordered_set<std::string> frameworkPaths;
    for (TypeResolvedFile const &library : inputLibraries) {
        if (library.fileType()->isFrameworkWrapper()) {
            frameworkPaths.insert(FSUtil::GetDirectoryName(library.filePath()));
        } else {
            libraryPaths.insert(FSUtil::GetDirectoryName(library.filePath()));
        }
    }
    for (std::string const &libraryPath : libraryPaths) {
        special.push_back("-L" + libraryPath);
    }
    for (std::string const &libraryPath : frameworkPaths) {
        special.push_back("-F" + libraryPath);
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

    std::unordered_set<std::string> removed;
    if (linker->identifier() == "com.apple.xcode.linkers.lipo") {
        // This is weird, but this flag is invalid yet is in the specification.
        removed.insert("-arch_only");
    }

    // TODO(grp): Do something more with this. Set $(DependencyInfoFile) in the environment?
    std::string dependencyInfo = linker->dependencyInfoFile();

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (linker);
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputFiles, { output });
    OptionsResult options = OptionsResult::Create(toolEnvironment, workingDirectory, nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, executable, special, removed);
    std::string logMessage = ToolInvocationContext::LogMessage(toolEnvironment);
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory, dependencyInfo, auxiliaries);
    return LinkerInvocationContext(context.invocation());
}
