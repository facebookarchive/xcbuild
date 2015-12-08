/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/LinkerResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::LinkerResolver;
using pbxbuild::Tool::ToolResult;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::ToolContext;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

LinkerResolver::
LinkerResolver(pbxspec::PBX::Linker::shared_ptr const &linker) :
    _linker(linker)
{
}

LinkerResolver::
~LinkerResolver()
{
}

void LinkerResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::vector<std::string> const &inputFiles,
    std::vector<TypeResolvedFile> const &inputLibraries,
    std::string const &output,
    std::vector<std::string> const &additionalArguments,
    std::string const &executable
)
{
    std::vector<std::string> special;
    std::vector<ToolInvocation::AuxiliaryFile> auxiliaries;

    special.insert(special.end(), additionalArguments.begin(), additionalArguments.end());

    if (_linker->supportsInputFileList() || _linker->identifier() == LinkerResolver::LibtoolToolIdentifier()) {
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
    if (_linker->identifier() == LinkerResolver::LipoToolIdentifier()) {
        // This is weird, but this flag is invalid yet is in the specification.
        removed.insert("-arch_only");
    }

    std::string dependencyInfo;
    if (_linker->identifier() == LinkerResolver::LinkerToolIdentifier()) {
        dependencyInfo = environment.expand(_linker->dependencyInfoFile());

        special.push_back("-Xlinker");
        special.push_back("-dependency_info");
        special.push_back("-Xlinker");
        special.push_back(dependencyInfo);
    }

    pbxspec::PBX::Tool::shared_ptr tool = std::static_pointer_cast <pbxspec::PBX::Tool> (_linker);
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputFiles, { output });
    OptionsResult options = OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, executable, special, removed);
    std::string logMessage = ToolResult::LogMessage(toolEnvironment);

    ToolInvocation invocation = ToolResult::CreateInvocation(toolEnvironment, options, commandLine, logMessage, toolContext->workingDirectory(), dependencyInfo, auxiliaries);
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<LinkerResolver> LinkerResolver::
Create(Phase::PhaseEnvironment const &phaseEnvironment, std::string const &identifier)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Linker::shared_ptr linker = buildEnvironment.specManager()->linker(identifier, targetEnvironment.specDomains());
    if (linker == nullptr) {
        fprintf(stderr, "warning: could not find linker %s\n", identifier.c_str());
        return nullptr;
    }

    return std::unique_ptr<LinkerResolver>(new LinkerResolver(linker));
}

