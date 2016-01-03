/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Tokens.h>
#include <pbxbuild/Tool/Context.h>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::CopyResolver::
CopyResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

void Tool::CopyResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<Phase::File> const &inputs,
    std::string const &outputDirectory,
    std::string const &logMessageTitle) const
{
    /*
     * Add the copy-specific build settings.
     */
    pbxsetting::Level copyLevel = pbxsetting::Level({
        pbxsetting::Setting::Create("PBXCP_STRIP_TOOL", pbxsetting::Value::String("")), // TODO(grp): Get path to strip.
        pbxsetting::Setting::Create("PBXCP_BITCODE_STRIP_TOOL", pbxsetting::Value::String("")), // TODO(grp): Get path to bitcode_strip.
        pbxsetting::Setting::Create("pbxcp_rule_name", pbxsetting::Value::String(logMessageTitle)),
    });

    pbxsetting::Environment environment = baseEnvironment;
    environment.insertFront(copyLevel, false);

    /*
     * The output path is the same as the input, but in the output directory.
     */
    std::vector<std::string> outputs;
    std::transform(inputs.begin(), inputs.end(), std::back_inserter(outputs), [&](Phase::File const &input) -> std::string {
        std::string output = outputDirectory + "/" + FSUtil::GetBaseName(input.path());
        return output;
    });

    /*
     * Build the arguments: each input file, then the output directory.
     */
    std::vector<std::string> args;
    for (Phase::File const &file : inputs) {
        args.push_back(file.path());
    }
    args.push_back(outputDirectory);

    /*
     * Resolve the tool options.
     */
    Tool::Environment toolEnvironment = Tool::Environment::Create(_tool, environment, toolContext->workingDirectory(), inputs, outputs);
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::Tokens commandLine = Tool::Tokens::CommandLine(toolEnvironment, options, std::string(), args);
    std::string logMessage = Tool::ToolResult::LogMessage(toolEnvironment);

    /*
     * Create the copy invocation.
     */
    Tool::Invocation invocation;
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
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr copyTool = buildEnvironment.specManager()->tool(Tool::CopyResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::CopyResolver>(new Tool::CopyResolver(copyTool));
}

