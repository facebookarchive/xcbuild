/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/ToolResolver.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <pbxbuild/Tool/ToolContext.h>

namespace Tool = pbxbuild::Tool;
using pbxbuild::ToolInvocation;

Tool::ToolResolver::
ToolResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

Tool::ToolResolver::
~ToolResolver()
{
}

void Tool::ToolResolver::
resolve(
    Tool::ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::vector<std::string> const &inputs,
    std::vector<std::string> const &outputs,
    std::string const &logMessage) const
{
    Tool::ToolEnvironment toolEnvironment = Tool::ToolEnvironment::Create(_tool, environment, inputs, outputs);
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::CommandLineResult commandLine = Tool::CommandLineResult::Create(toolEnvironment, options);
    std::string resolvedLogMessage = (!logMessage.empty() ? logMessage : ToolResult::LogMessage(toolEnvironment));

    ToolInvocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.logMessage() = resolvedLogMessage;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::ToolResolver> Tool::ToolResolver::
Create(Phase::Environment const &phaseEnvironment, std::string const &identifier)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr tool = nullptr;
    if (pbxspec::PBX::Tool::shared_ptr tool_ = buildEnvironment.specManager()->tool(identifier, targetEnvironment.specDomains())) {
        tool = tool_;
    } else if (pbxspec::PBX::Compiler::shared_ptr compiler = buildEnvironment.specManager()->compiler(identifier, targetEnvironment.specDomains())) {
        tool = std::static_pointer_cast<pbxspec::PBX::Tool>(compiler);
    } else if (pbxspec::PBX::Linker::shared_ptr linker = buildEnvironment.specManager()->linker(identifier, targetEnvironment.specDomains())) {
        tool = std::static_pointer_cast<pbxspec::PBX::Tool>(linker);
    } else {
        fprintf(stderr, "warning: could not find tool %s\n", identifier.c_str());
        return nullptr;
    }

    return std::unique_ptr<Tool::ToolResolver>(new Tool::ToolResolver(tool));
}
