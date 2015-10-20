// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <sstream>

using pbxbuild::Tool::ToolInvocationContext;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::ToolInvocation;
using pbxbuild::Tool::SearchPaths;
using libutil::FSUtil;

ToolInvocationContext::
ToolInvocationContext(ToolInvocation const &invocation) :
    _invocation(invocation)
{
}

ToolInvocationContext::
~ToolInvocationContext()
{
}

std::string ToolInvocationContext::
LogMessage(ToolEnvironment const &toolEnvironment)
{
    pbxsetting::Environment const &environment = toolEnvironment.toolEnvironment();
    std::string ruleName = environment.expand(toolEnvironment.tool()->ruleName());
    std::string ruleFormat = environment.expand(toolEnvironment.tool()->ruleFormat());

    if (!ruleName.empty()) {
        return ruleName;
    } else if (!ruleFormat.empty()) {
        return ruleFormat;
    } else {
        return std::string();
    }
}

ToolInvocationContext ToolInvocationContext::
Create(
    ToolEnvironment const &toolEnvironment,
    OptionsResult const &options,
    CommandLineResult const &commandLine,
    std::string const &logMessage,
    std::string const &workingDirectory,
    std::string const &dependencyInfo,
    std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles
)
{
    pbxbuild::ToolInvocation invocation = pbxbuild::ToolInvocation(
        commandLine.executable(),
        commandLine.arguments(),
        options.environment(),
        workingDirectory,
        toolEnvironment.inputs(),
        toolEnvironment.outputs(),
        dependencyInfo,
        auxiliaryFiles,
        logMessage
    );
    return ToolInvocationContext(invocation);
}

ToolInvocationContext ToolInvocationContext::
Create(
    pbxspec::PBX::Tool::shared_ptr const &tool,
    std::vector<std::string> const &inputs,
    std::vector<std::string> const &outputs,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &logMessage
)
{
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(tool, environment, inputs, outputs);
    OptionsResult options = OptionsResult::Create(toolEnvironment, workingDirectory, nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options);
    std::string resolvedLogMessage = (!logMessage.empty() ? logMessage : ToolInvocationContext::LogMessage(toolEnvironment));
    return ToolInvocationContext::Create(toolEnvironment, options, commandLine, resolvedLogMessage, workingDirectory);
}
