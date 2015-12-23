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

using pbxbuild::Tool::ToolResolver;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::ToolResult;
using pbxbuild::ToolInvocation;
using pbxbuild::Tool::SearchPaths;

ToolResolver::
ToolResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

ToolResolver::
~ToolResolver()
{
}

void ToolResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::vector<std::string> const &inputs,
    std::vector<std::string> const &outputs,
    std::string const &logMessage) const
{
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(_tool, environment, inputs, outputs);
    OptionsResult options = OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options);
    std::string resolvedLogMessage = (!logMessage.empty() ? logMessage : ToolResult::LogMessage(toolEnvironment));

    ToolInvocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs();
    invocation.outputs() = toolEnvironment.outputs();
    invocation.logMessage() = resolvedLogMessage;
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<ToolResolver> ToolResolver::
Create(Phase::PhaseEnvironment const &phaseEnvironment, std::string const &identifier)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

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

    return std::unique_ptr<ToolResolver>(new ToolResolver(tool));
}
