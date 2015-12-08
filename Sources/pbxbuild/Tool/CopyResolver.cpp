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

using pbxbuild::Tool::CopyResolver;
using pbxbuild::Tool::ToolResult;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::ToolInvocation;
using libutil::FSUtil;

CopyResolver::
CopyResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

void CopyResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &inputFile,
    std::string const &outputDirectory,
    std::string const &logMessageTitle
) const
{
    std::string outputFile = outputDirectory + "/" + FSUtil::GetBaseName(inputFile);
    std::string logMessage = logMessageTitle + " " + FSUtil::GetRelativePath(inputFile, toolContext->workingDirectory()) + " " + outputFile;

    ToolEnvironment toolEnvironment = ToolEnvironment::Create(_tool, environment, { inputFile }, { outputFile });
    OptionsResult options = OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "", { inputFile, outputDirectory });

    ToolInvocation invocation = ToolResult::CreateInvocation(toolEnvironment, options, commandLine, logMessage, toolContext->workingDirectory());
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<CopyResolver> CopyResolver::
Create(Phase::PhaseEnvironment const &phaseEnvironment)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr copyTool = buildEnvironment.specManager()->tool(CopyResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    return std::unique_ptr<CopyResolver>(new CopyResolver(copyTool));
}

