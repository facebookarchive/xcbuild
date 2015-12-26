/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/InfoPlistResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::InfoPlistResolver;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::Tool::ToolResult;
using pbxbuild::ToolInvocation;

InfoPlistResolver::
InfoPlistResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

InfoPlistResolver::
~InfoPlistResolver()
{
}

void InfoPlistResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &input) const
{
    bool pkginfoFile = pbxsetting::Type::ParseBoolean(environment.resolve("GENERATE_PKGINFO_FILE"));

    std::string additionalContentPaths;
    for (std::string const &additionalContentPath : toolContext->additionalInfoPlistContents()) {
        if (&additionalContentPath != &toolContext->additionalInfoPlistContents()[0]) {
            additionalContentPaths += " ";
        }
        additionalContentPaths += additionalContentPath;
    }

    pbxsetting::Level level = pbxsetting::Level({
        pbxsetting::Setting::Parse("GeneratedPkgInfoFile", (pkginfoFile ? "$(TARGET_BUILD_DIR)/$(PKGINFO_PATH)" : "")),
        pbxsetting::Setting::Parse("ExpandBuildSettings", "$(INFOPLIST_EXPAND_BUILD_SETTINGS)"),
        pbxsetting::Setting::Parse("OutputFormat", "$(INFOPLIST_OUTPUT_FORMAT)"),
        pbxsetting::Setting::Parse("AdditionalContentFilePaths", additionalContentPaths),
        pbxsetting::Setting::Parse("RequiredArchitectures", ""), // TODO(grp): Determine what this is for.
        pbxsetting::Setting::Parse("AdditionalInfoFileKeys", ""), // TODO(grp): Determine what these are for.
        pbxsetting::Setting::Parse("AdditionalInfoFileValues", ""), // TODO(grp): Determine what these are for.
    });

    pbxsetting::Environment env = environment;
    env.insertFront(level, false);

    std::string infoPlistPath = environment.resolve("TARGET_BUILD_DIR") + "/" + environment.resolve("INFOPLIST_PATH");

    ToolEnvironment toolEnvironment = ToolEnvironment::Create(_tool, env, { toolContext->workingDirectory() + "/" + input }, { infoPlistPath });
    OptionsResult options = OptionsResult::Create(toolEnvironment, toolContext->workingDirectory(), nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, std::string());
    std::string logMessage = ToolResult::LogMessage(toolEnvironment);

    /* Pass all build settings for expansion. */
    std::unordered_map<std::string, std::string> environmentVariables = options.environment();
    std::unordered_map<std::string, std::string> buildSettingValues = environment.computeValues(pbxsetting::Condition::Empty());
    environmentVariables.insert(buildSettingValues.begin(), buildSettingValues.end());

    bool showEnvironmentInLog = false;

    ToolInvocation invocation;
    invocation.executable() = commandLine.executable();
    invocation.arguments() = commandLine.arguments();
    invocation.environment() = environmentVariables;
    invocation.workingDirectory() = toolContext->workingDirectory();
    invocation.inputs() = toolEnvironment.inputs(toolContext->workingDirectory());
    invocation.outputs() = toolEnvironment.outputs(toolContext->workingDirectory());
    invocation.inputDependencies() = toolContext->additionalInfoPlistContents();
    invocation.logMessage() = logMessage;
    invocation.showEnvironmentInLog() = false; /* Hide build settings from log. */
    toolContext->invocations().push_back(invocation);
}

std::unique_ptr<InfoPlistResolver> InfoPlistResolver::
Create(Phase::Environment const &phaseEnvironment)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr infoPlistTool = buildEnvironment.specManager()->tool(InfoPlistResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (infoPlistTool == nullptr) {
        fprintf(stderr, "warning: could not find info plist tool\n");
        return nullptr;
    }

    return std::unique_ptr<InfoPlistResolver>(new InfoPlistResolver(infoPlistTool));
}
