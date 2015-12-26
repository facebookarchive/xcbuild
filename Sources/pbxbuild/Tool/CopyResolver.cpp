/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/TypeResolvedFile.h>

namespace Tool = pbxbuild::Tool;
using pbxbuild::ToolInvocation;
using libutil::FSUtil;

Tool::CopyResolver::
CopyResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

void Tool::CopyResolver::
resolve(
    Tool::ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &inputFile,
    std::string const &outputDirectory,
    std::string const &logMessageTitle
) const
{
    std::string outputFile = outputDirectory + "/" + FSUtil::GetBaseName(inputFile);
    std::string logMessage = logMessageTitle + " " + FSUtil::GetRelativePath(inputFile, toolContext->workingDirectory()) + " " + outputFile;

    Tool::ToolEnvironment toolEnvironment = Tool::ToolEnvironment::Create(_tool, environment, { inputFile }, { outputFile });
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::CommandLineResult commandLine = Tool::CommandLineResult::Create(toolEnvironment, options, "", { inputFile, outputDirectory });

    ToolInvocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.logMessage() = logMessage;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::CopyResolver> Tool::CopyResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr copyTool = buildEnvironment.specManager()->tool(Tool::CopyResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::CopyResolver>(new Tool::CopyResolver(copyTool));
}

