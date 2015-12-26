/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/TouchResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/Context.h>
#include <pbxbuild/TypeResolvedFile.h>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::TouchResolver::
TouchResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

Tool::TouchResolver::
~TouchResolver()
{
}

void Tool::TouchResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &input,
    std::vector<std::string> const &dependencies) const
{
    std::string logMessage = "Touch " + input;

    /* Treat the input as an output since it's what gets modified by the touch. */
    Tool::Environment toolEnvironment = Tool::Environment::Create(_tool, environment, { }, { });
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::CommandLineResult commandLine = Tool::CommandLineResult::Create(toolEnvironment, options, "/usr/bin/touch", { "-c", input });

    std::vector<std::string> inputDependencies;
    for (std::string const &dependency : dependencies) {
        inputDependencies.push_back(FSUtil::ResolveRelativePath(dependency, toolContext->workingDirectory()));
    }

    std::string outputPath = FSUtil::ResolveRelativePath(input, toolContext->workingDirectory());

    Tool::Invocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.phonyOutputs() = { outputPath }; /* Not created, just updated. */
    invocation.inputDependencies() = inputDependencies;
    invocation.logMessage() = logMessage;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::TouchResolver> Tool::TouchResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr touchTool = buildEnvironment.specManager()->tool(Tool::TouchResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (touchTool == nullptr) {
        fprintf(stderr, "warning: could not find touch tool\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::TouchResolver>(new Tool::TouchResolver(touchTool));
}
