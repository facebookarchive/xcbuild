/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/CopyResolver.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Tokens.h>
#include <pbxbuild/Tool/Context.h>
#include <libutil/FSUtil.h>

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
        pbxsetting::Setting::Create("PBXCP_STRIP_TOOL", FSUtil::FindExecutable("strip", toolContext->executablePaths())),
        pbxsetting::Setting::Create("PBXCP_BITCODE_STRIP_TOOL", FSUtil::FindExecutable("bitcode_strip", toolContext->executablePaths())),
        pbxsetting::Setting::Create("pbxcp_rule_name", logMessageTitle),
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
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options, std::string(), args);

    // TODO(grp): This should be generic for all tools.
    std::vector<Tool::Invocation::DependencyInfo> dependencyInfo;
    if (_tool->deeplyStatInputDirectories()) {
        for (Phase::File const &input : inputs) {
            /* Create a dependency info file to track the input directory contents. */
            auto info = Tool::Invocation::DependencyInfo(dependency::DependencyInfoFormat::Directory, input.path());
            dependencyInfo.push_back(info);
        }
    }

    /*
     * Create the copy invocation.
     */
    Tool::Invocation invocation;
    invocation.executable() = Tool::Invocation::Executable::Determine(tokens.executable(), toolContext->executablePaths());
    invocation.arguments() = tokens.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.dependencyInfo() = dependencyInfo;
    invocation.logMessage() = tokens.logMessage();
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

