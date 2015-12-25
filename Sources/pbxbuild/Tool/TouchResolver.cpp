/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/TouchResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::TouchResolver;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::Tool::ToolResult;
using pbxbuild::ToolInvocation;
using libutil::FSUtil;

TouchResolver::
TouchResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

TouchResolver::
~TouchResolver()
{
}

void TouchResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &input,
    std::vector<std::string> const &dependencies) const
{
    std::string logMessage = "Touch " + input;

    /* Treat the input as an output since it's what gets modified by the touch. */
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(_tool, environment, { }, { });
    OptionsResult options = OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "/usr/bin/touch", { "-c", input });

    std::vector<std::string> inputDependencies;
    for (std::string const &dependency : dependencies) {
        inputDependencies.push_back(FSUtil::ResolveRelativePath(dependency, toolContext->workingDirectory()));
    }

    std::string outputPath = FSUtil::ResolveRelativePath(input, toolContext->workingDirectory());

    ToolInvocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.phonyOutputs() = { outputPath }; /* Not created, just updated. */
    invocation.inputDependencies() = inputDependencies;
    invocation.logMessage() = logMessage;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<TouchResolver> TouchResolver::
Create(Phase::PhaseEnvironment const &phaseEnvironment)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr touchTool = buildEnvironment.specManager()->tool(TouchResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (touchTool == nullptr) {
        fprintf(stderr, "warning: could not find touch tool\n");
        return nullptr;
    }

    return std::unique_ptr<TouchResolver>(new TouchResolver(touchTool));
}
