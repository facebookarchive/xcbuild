/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/InterfaceBuilderStoryboardLinkerResolver.h>
#include <pbxbuild/Tool/InterfaceBuilderCommon.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Tokens.h>
#include <pbxbuild/Tool/Context.h>
#include <libutil/FSUtil.h>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::InterfaceBuilderStoryboardLinkerResolver::
InterfaceBuilderStoryboardLinkerResolver(pbxspec::PBX::Compiler::shared_ptr const &tool) :
    _tool(tool)
{
}

void Tool::InterfaceBuilderStoryboardLinkerResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<std::string> const &inputs) const
{
    /*
     * Create the custom environment with the tool options.
     */
    pbxsetting::Level level = pbxsetting::Level({
        InterfaceBuilderCommon::TargetedDeviceSetting(baseEnvironment),
    });
    pbxsetting::Environment interfaceBuilderEnvironment = pbxsetting::Environment(baseEnvironment);
    interfaceBuilderEnvironment.insertFront(level, false);

    /*
     * Resolve the tool options.
     */
    Tool::Environment toolEnvironment = Tool::Environment::Create(_tool, interfaceBuilderEnvironment, toolContext->workingDirectory(), inputs);
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options);

    pbxsetting::Environment const &environment = toolEnvironment.environment();

    /*
     * Determine the output path for the inputs.
     */
    std::string tempDirectory = environment.resolve("TempResourcesDir");
    std::string resourcesDirectory = environment.resolve("ProductResourcesDir");
    std::vector<std::string> outputs;
    for (std::string const &input : inputs) {
        /* This assumes the inputs all come from TempResourcesDir, which should be true. */
        std::string relative = FSUtil::GetRelativePath(input, tempDirectory);
        std::string output = resourcesDirectory + "/" + relative;
        outputs.push_back(output);
    }

    /*
     * Add custom arguments to the end.
     */
    std::vector<std::string> arguments = tokens.arguments();
    std::vector<std::string> deploymentTargetArguments = InterfaceBuilderCommon::DeploymentTargetArguments(environment);
    arguments.insert(arguments.end(), deploymentTargetArguments.begin(), deploymentTargetArguments.end());

    // TODO(grp): These should be handled generically for all tools.
    std::unordered_map<std::string, std::string> environmentVariables = options.environment();
    if (_tool->environmentVariables()) {
        for (auto const &variable : *_tool->environmentVariables()) {
            environmentVariables.insert({ variable.first, environment.expand(variable.second) });
        }
    }

    /*
     * Create the invocation.
     */
    Tool::Invocation invocation;
    invocation.executable() = Tool::Invocation::Executable::Determine(tokens.executable());
    invocation.arguments() = arguments;
    invocation.environment() = environmentVariables;
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = outputs;
    invocation.logMessage() = tokens.logMessage();
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::InterfaceBuilderStoryboardLinkerResolver> Tool::InterfaceBuilderStoryboardLinkerResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Compiler::shared_ptr interfaceBuilderTool = buildEnvironment.specManager()->compiler(Tool::InterfaceBuilderStoryboardLinkerResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (interfaceBuilderTool == nullptr) {
        fprintf(stderr, "warning: could not find interface builder storyboard linker tool\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::InterfaceBuilderStoryboardLinkerResolver>(new Tool::InterfaceBuilderStoryboardLinkerResolver(interfaceBuilderTool));
}

