/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/LinkerResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/TypeResolvedFile.h>

namespace Tool = pbxbuild::Tool;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

Tool::LinkerResolver::
LinkerResolver(pbxspec::PBX::Linker::shared_ptr const &linker) :
    _linker(linker)
{
}

Tool::LinkerResolver::
~LinkerResolver()
{
}

void Tool::LinkerResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &environment,
    std::vector<std::string> const &inputFiles,
    std::vector<TypeResolvedFile> const &inputLibraries,
    std::string const &output,
    std::vector<std::string> const &additionalArguments,
    std::string const &executable
)
{
    std::vector<std::string> special;
    std::vector<Tool::Invocation::AuxiliaryFile> auxiliaries;

    special.insert(special.end(), additionalArguments.begin(), additionalArguments.end());

    if (_linker->supportsInputFileList() || _linker->identifier() == Tool::LinkerResolver::LibtoolToolIdentifier()) {
        std::string path = environment.expand(pbxsetting::Value::Parse("$(LINK_FILE_LIST_$(variant)_$(arch))"));
        std::string contents;
        for (std::string const &input : inputFiles) {
            contents += input + "\n";
        }
        Tool::Invocation::AuxiliaryFile fileList = Tool::Invocation::AuxiliaryFile(path, contents, false);
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
    if (_linker->identifier() == Tool::LinkerResolver::LipoToolIdentifier()) {
        // This is weird, but this flag is invalid yet is in the specification.
        removed.insert("-arch_only");
    }

    std::string dependencyInfo;
    if (_linker->identifier() == Tool::LinkerResolver::LinkerToolIdentifier()) {
        dependencyInfo = environment.expand(_linker->dependencyInfoFile());

        special.push_back("-Xlinker");
        special.push_back("-dependency_info");
        special.push_back("-Xlinker");
        special.push_back(dependencyInfo);
    }

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (_linker);
    Tool::Environment toolEnvironment = Tool::Environment::Create(tool, environment, inputFiles, { output });
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::CommandLineResult commandLine = Tool::CommandLineResult::Create(toolEnvironment, options, executable, special, removed);
    std::string logMessage = Tool::ToolResult::LogMessage(toolEnvironment);

    Tool::Invocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.auxiliaryFiles() = auxiliaries;
    invocation.dependencyInfo() = dependencyInfo;
    invocation.logMessage() = logMessage;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::LinkerResolver> Tool::LinkerResolver::
Create(Phase::Environment const &phaseEnvironment, std::string const &identifier)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Linker::shared_ptr linker = buildEnvironment.specManager()->linker(identifier, targetEnvironment.specDomains());
    if (linker == nullptr) {
        fprintf(stderr, "warning: could not find linker %s\n", identifier.c_str());
        return nullptr;
    }

    return std::unique_ptr<Tool::LinkerResolver>(new Tool::LinkerResolver(linker));
}

