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
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

#include <algorithm>

namespace Tool = pbxbuild::Tool;
namespace Phase = pbxbuild::Phase;
using libutil::Filesystem;
using libutil::FSUtil;

Tool::CopyResolver::
CopyResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

static void
ResolveInternal(
    pbxspec::PBX::Tool::shared_ptr const &tool,
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<std::string> const &inputPaths,
    ext::optional<std::vector<Phase::File>> const &inputs,
    std::string const &outputDirectory,
    std::string const &logMessageTitle)
{
    /*
     * Add the copy-specific build settings.
     */
    pbxsetting::Level copyLevel = pbxsetting::Level({
        // TODO: pbxsetting::Setting::Create("PBXCP_STRIP_TOOL", toolchain->path() + "/usr/bin/strip"),
        // TODO: pbxsetting::Setting::Create("PBXCP_BITCODE_STRIP_TOOL", toolchain->path() + "/usr/bin/bitcode_strip"),
        pbxsetting::Setting::Create("pbxcp_rule_name", logMessageTitle),
    });

    pbxsetting::Environment environment = pbxsetting::Environment(baseEnvironment);
    environment.insertFront(copyLevel, false);

    /*
     * The output path is the same as the input, but in the output directory.
     */
    std::vector<std::string> outputPaths;
    for (std::string const &inputPath : inputPaths) {
        std::string outputPath = outputDirectory + "/" + FSUtil::GetBaseName(inputPath);
        outputPaths.push_back(outputPath);
    }

    /*
     * Build the arguments: each input file, then the output directory.
     */
    std::vector<std::string> args;
    for (std::string const &inputPath : inputPaths) {
        args.push_back(inputPath);
    }
    args.push_back(outputDirectory);

    /*
     * Resolve the tool options. Inputs can either be full build files or just paths.
     */
    Tool::Environment toolEnvironment = (inputs ?
        Tool::Environment::Create(tool, environment, toolContext->workingDirectory(), *inputs, outputPaths) :
        Tool::Environment::Create(tool, environment, toolContext->workingDirectory(), inputPaths, outputPaths));
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options, std::string(), args);

    // TODO(grp): This should be generic for all tools.
    std::vector<Tool::Invocation::DependencyInfo> dependencyInfo;
    if (tool->deeplyStatInputDirectories()) {
        for (std::string const &inputPath : inputPaths) {
            /* Create a dependency info file to track the input directory contents. */
            auto info = Tool::Invocation::DependencyInfo(dependency::DependencyInfoFormat::Directory, inputPath);
            dependencyInfo.push_back(info);
        }
    }

    /*
     * Create the copy invocation.
     */
    Tool::Invocation invocation;
    invocation.executable() = Tool::Invocation::Executable::Determine(tokens.executable());
    invocation.arguments() = tokens.arguments();
    invocation.environment() = options.environment();
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.dependencyInfo() = dependencyInfo;
    invocation.logMessage() = tokens.logMessage();
    toolContext->invocations().push_back(invocation);
}

void Tool::CopyResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<std::string> const &inputs,
    std::string const &outputDirectory,
    std::string const &logMessageTitle) const
{
    return ResolveInternal(
        _tool,
        toolContext,
        baseEnvironment,
        inputs,
        ext::nullopt,
        outputDirectory,
        logMessageTitle);
}

void Tool::CopyResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<Phase::File> const &inputs,
    std::string const &outputDirectory,
    std::string const &logMessageTitle) const
{
    std::vector<std::string> inputPaths;
    std::transform(inputs.begin(), inputs.end(), std::back_inserter(inputPaths), [&](Phase::File const &input) -> std::string {
        return input.path();
    });

    ResolveInternal(
        _tool,
        toolContext,
        baseEnvironment,
        inputPaths,
        inputs,
        outputDirectory,
        logMessageTitle);
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

