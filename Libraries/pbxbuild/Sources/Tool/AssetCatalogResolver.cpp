/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/AssetCatalogResolver.h>
#include <pbxbuild/Tool/InterfaceBuilderCommon.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Tokens.h>
#include <pbxbuild/Tool/Context.h>

namespace Tool = pbxbuild::Tool;

Tool::AssetCatalogResolver::
AssetCatalogResolver(pbxspec::PBX::Compiler::shared_ptr const &tool) :
    _tool(tool)
{
}

void Tool::AssetCatalogResolver::
resolve(
    Tool::Context *toolContext,
    pbxsetting::Environment const &baseEnvironment,
    std::vector<Phase::File> const &inputs) const
{
    /*
     * Create the custom environment with the tool options.
     */
    pbxsetting::Level level = pbxsetting::Level({
        InterfaceBuilderCommon::TargetedDeviceSetting(baseEnvironment),
    });
    pbxsetting::Environment assetCatalogEnvironment = pbxsetting::Environment(baseEnvironment);
    assetCatalogEnvironment.insertFront(level, false);

    /*
     * Resolve the tool options.
     */
    Tool::Environment toolEnvironment = Tool::Environment::Create(_tool, assetCatalogEnvironment, toolContext->workingDirectory(), inputs);
    Tool::OptionsResult options = Tool::OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    Tool::Tokens::ToolExpansions tokens = Tool::Tokens::ExpandTool(toolEnvironment, options);

    pbxsetting::Environment const &environment = toolEnvironment.environment();

    /*
     * Create tool outputs.
     */
    std::vector<std::string> outputs = {
        /* Creates the asset catalog, always in the resources dir. */
        environment.expand(pbxsetting::Value::Parse("$(ProductResourcesDir)/Assets.car")),
    };

    /*
     * Add custom arguments to the end.
     */
    std::vector<std::string> arguments = tokens.arguments();
    arguments.push_back("--platform");
    arguments.push_back(environment.resolve("PLATFORM_NAME"));
    std::vector<std::string> deploymentTargetArguments = InterfaceBuilderCommon::DeploymentTargetArguments(environment);
    arguments.insert(arguments.end(), deploymentTargetArguments.begin(), deploymentTargetArguments.end());

    // TODO(grp): This is a hack to work around missing `Condition` support in options.
    arguments.erase(std::remove(arguments.begin(), arguments.end(), "--optimization"), arguments.end());
    arguments.erase(std::remove(arguments.begin(), arguments.end(), ""), arguments.end());

    // TODO(grp): These should be handled generically for all tools.
    std::unordered_map<std::string, std::string> environmentVariables = options.environment();
    if (_tool->environmentVariables()) {
        for (auto const &variable : *_tool->environmentVariables()) {
            environmentVariables.insert({ variable.first, environment.expand(variable.second) });
        }
    }

    // TODO(grp): This should be handled generically for all tools.
    if (_tool->generatedInfoPlistContentFilePath()) {
        std::string infoPlistContent = environment.expand(*_tool->generatedInfoPlistContentFilePath());
        toolContext->additionalInfoPlistContents().push_back(infoPlistContent);
        outputs.push_back(infoPlistContent);
    }

    // TODO(grp): This should be handled generically for all tools.
    std::vector<Tool::Invocation::DependencyInfo> dependencyInfo;
    if (_tool->dependencyInfoFile()) {
        dependencyInfo.push_back(Tool::Invocation::DependencyInfo(
            dependency::DependencyInfoFormat::Binary,
            environment.expand(*_tool->dependencyInfoFile())));
    }
    if (_tool->deeplyStatInputDirectories()) {
        for (Phase::File const &input : inputs) {
            /* Create a dependency info file to track the input directory contents. */
            auto info = Tool::Invocation::DependencyInfo(dependency::DependencyInfoFormat::Directory, input.path());
            dependencyInfo.push_back(info);
        }
    }

    /*
     * Create the asset catalog invocation.
     */
    Tool::Invocation invocation;
    invocation.executable() = Tool::Invocation::Executable::Determine(tokens.executable());
    invocation.arguments() = arguments;
    invocation.environment() = environmentVariables;
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = outputs;
    invocation.dependencyInfo() = dependencyInfo;
    invocation.logMessage() = tokens.logMessage();
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<Tool::AssetCatalogResolver> Tool::AssetCatalogResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Compiler::shared_ptr assetCatalogTool = buildEnvironment.specManager()->compiler(Tool::AssetCatalogResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (assetCatalogTool == nullptr) {
        fprintf(stderr, "warning: could not find asset catalog compiler\n");
        return nullptr;
    }

    return std::unique_ptr<Tool::AssetCatalogResolver>(new Tool::AssetCatalogResolver(assetCatalogTool));
}

