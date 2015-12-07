/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <sstream>

using pbxbuild::Tool::ToolResult;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::ToolInvocation;

std::string ToolResult::
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

ToolInvocation ToolResult::
CreateInvocation(
    ToolEnvironment const &toolEnvironment,
    OptionsResult const &options,
    CommandLineResult const &commandLine,
    std::string const &logMessage,
    std::string const &workingDirectory,
    std::string const &dependencyInfo,
    std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles
)
{
    return pbxbuild::ToolInvocation(
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
}

