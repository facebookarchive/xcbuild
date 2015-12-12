/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/ScriptResolver.h>
#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/TypeResolvedFile.h>

using pbxbuild::Tool::ScriptResolver;
using pbxbuild::Tool::ToolResult;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::CommandLineResult;
using pbxbuild::ToolInvocation;
using pbxbuild::TypeResolvedFile;
using libutil::FSUtil;

ScriptResolver::
ScriptResolver(pbxspec::PBX::Tool::shared_ptr const &tool) :
    _tool(tool)
{
}

static pbxsetting::Level
ScriptInputOutputLevel(std::vector<std::string> const &inputFiles, std::vector<std::string> const &outputFiles, bool multipleInputs)
{
    std::vector<pbxsetting::Setting> settings;

    settings.push_back(pbxsetting::Setting::Parse("SCRIPT_OUTPUT_FILE_COUNT", std::to_string(outputFiles.size())));
    for (auto it = outputFiles.begin(); it < outputFiles.end(); ++it) {
        size_t index = (it - outputFiles.begin());
        settings.push_back(pbxsetting::Setting::Parse("SCRIPT_OUTPUT_FILE_" + std::to_string(index), *it));
    }

    if (multipleInputs) {
        settings.push_back(pbxsetting::Setting::Parse("SCRIPT_INPUT_FILE_COUNT", std::to_string(inputFiles.size())));
        for (auto it = inputFiles.begin(); it < inputFiles.end(); ++it) {
            size_t index = (it - inputFiles.begin());
            settings.push_back(pbxsetting::Setting::Parse("SCRIPT_INPUT_FILE_" + std::to_string(index), *it));
        }
    } else if (!inputFiles.empty()) {
        settings.push_back(pbxsetting::Setting::Parse("SCRIPT_INPUT_FILE", inputFiles.front()));
    }

    return pbxsetting::Level(settings);
}

void ScriptResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &shell,
    std::vector<std::string> const &arguments,
    std::unordered_map<std::string, std::string> const &environmentVariables,
    std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaries,
    std::vector<std::string> const &inputFiles,
    std::vector<std::string> const &outputFiles,
    std::string const &logMessage,
    bool showEnvironmentInLog) const
{
    ToolEnvironment toolEnvironment = ToolEnvironment::Create(_tool, environment, inputFiles, outputFiles);
    OptionsResult options = OptionsResult::Create(toolEnvironment, workingDirectory, nullptr, environmentVariables);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, shell, arguments);

    ToolInvocation invocation = ToolInvocation(
        commandLine.executable(),
        commandLine.arguments(),
        options.environment(),
        workingDirectory,
        toolEnvironment.inputs(),
        toolEnvironment.outputs(),
        std::string(),
        auxiliaries,
        logMessage,
        showEnvironmentInLog
    );
    toolContext->invocations().push_back(invocation);
}

void ScriptResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget) const
{
    std::string logMessage = "ExternalBuildToolExecution " + legacyTarget->name();

    std::string script = environment.expand(legacyTarget->buildArgumentsString());

    std::unordered_map<std::string, std::string> environmentVariables;
    if (legacyTarget->passBuildSettingsInEnvironment()) {
        environmentVariables = environment.computeValues(pbxsetting::Condition::Empty());
    }

    std::string fullWorkingDirectory = toolContext->workingDirectory() + "/" + legacyTarget->buildWorkingDirectory();

    resolve(
        toolContext,
        environment,
        fullWorkingDirectory,
        legacyTarget->buildToolPath(),
        pbxsetting::Type::ParseList(script),
        environmentVariables,
        { },
        { },
        { },
        logMessage,
        true
    );
}

void ScriptResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase) const
{
    pbxsetting::Level level = pbxsetting::Level({
        pbxsetting::Setting::Parse("BuildPhaseName", (!buildPhase->name().empty() ? buildPhase->name() : "Run Script")),
        pbxsetting::Setting::Parse("BuildPhaseIdentifier", buildPhase->blueprintIdentifier()),
    });

    pbxsetting::Environment phaseEnvironment = environment;
    phaseEnvironment.insertFront(level, false);

    pbxsetting::Value scriptPath = pbxsetting::Value::Parse("$(TEMP_FILES_DIR)/Script-$(BuildPhaseIdentifier).sh");
    pbxsetting::Value logMessage = pbxsetting::Value::Parse("PhaseScriptExecution $(BuildPhaseName:quote) ") + scriptPath;

    std::vector<std::string> inputFiles;
    std::transform(buildPhase->inputPaths().begin(), buildPhase->inputPaths().end(), std::back_inserter(inputFiles), [&](pbxsetting::Value const &input) -> std::string {
        return environment.expand(input);
    });

    std::vector<std::string> outputFiles;
    std::transform(buildPhase->outputPaths().begin(), buildPhase->outputPaths().end(), std::back_inserter(outputFiles), [&](pbxsetting::Value const &output) -> std::string {
        return environment.expand(output);
    });

    std::string scriptFilePath = phaseEnvironment.expand(scriptPath);
    std::string contents = (!buildPhase->shellPath().empty() ? "#!" + buildPhase->shellPath() + "\n" : "") + buildPhase->shellScript();
    ToolInvocation::AuxiliaryFile scriptFile = ToolInvocation::AuxiliaryFile(scriptFilePath, contents, true);

    pbxsetting::Environment scriptEnvironment = environment;
    scriptEnvironment.insertFront(ScriptInputOutputLevel(inputFiles, outputFiles, true), false);
    std::unordered_map<std::string, std::string> environmentVariables = scriptEnvironment.computeValues(pbxsetting::Condition::Empty());

    resolve(
        toolContext,
        scriptEnvironment,
        toolContext->workingDirectory(),
        "/bin/sh",
        { "-c", scriptFilePath },
        environmentVariables,
        { scriptFile },
        inputFiles,
        outputFiles,
        phaseEnvironment.expand(logMessage),
        buildPhase->showEnvVarsInLog()
    );
}

void ScriptResolver::
resolve(
    ToolContext *toolContext,
    pbxsetting::Environment const &environment,
    std::string const &inputFile,
    pbxbuild::TargetBuildRules::BuildRule::shared_ptr const &buildRule) const
{
    pbxsetting::Level level = pbxsetting::Level({
        pbxsetting::Setting::Parse("INPUT_FILE_DIR", FSUtil::GetDirectoryName(inputFile)),
        pbxsetting::Setting::Parse("INPUT_FILE_BASE", FSUtil::GetBaseNameWithoutExtension(inputFile)),
        pbxsetting::Setting::Parse("INPUT_FILE_NAME", FSUtil::GetBaseName(inputFile)),
        pbxsetting::Setting::Parse("INPUT_FILE_PATH", inputFile),
        pbxsetting::Setting::Parse("INPUT_FILE_SUFFIX", (!FSUtil::GetFileExtension(inputFile).empty() ? "." + FSUtil::GetFileExtension(inputFile) : "")),
        // TODO(grp): INPUT_FILE_REGION_PATH_COMPONENT
    });

    pbxsetting::Environment ruleEnvironment = environment;
    ruleEnvironment.insertFront(level, false);

    pbxsetting::Value logMessage = pbxsetting::Value::Parse("RuleScriptExecution " + FSUtil::GetRelativePath(inputFile, toolContext->workingDirectory()) + " $(variant) $(arch)");

    std::vector<std::string> outputFiles;
    std::transform(buildRule->outputFiles().begin(), buildRule->outputFiles().end(), std::back_inserter(outputFiles), [&](pbxsetting::Value const &output) -> std::string {
        return ruleEnvironment.expand(output);
    });

    std::vector<std::string> inputFiles = { inputFile };

    ruleEnvironment.insertFront(ScriptInputOutputLevel(inputFiles, outputFiles, false), false);
    std::unordered_map<std::string, std::string> environmentVariables = ruleEnvironment.computeValues(pbxsetting::Condition::Empty());

    resolve(
        toolContext,
        ruleEnvironment,
        toolContext->workingDirectory(),
        "/bin/sh",
        { "-c", buildRule->script() },
        ruleEnvironment.computeValues(pbxsetting::Condition::Empty()),
        { },
        inputFiles,
        outputFiles,
        ruleEnvironment.expand(logMessage),
        true
    );
}

std::unique_ptr<ScriptResolver> ScriptResolver::
Create(Phase::PhaseEnvironment const &phaseEnvironment)
{
    pbxbuild::BuildEnvironment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    pbxbuild::TargetEnvironment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    pbxspec::PBX::Tool::shared_ptr scriptTool = buildEnvironment.specManager()->tool(ScriptResolver::ToolIdentifier(), targetEnvironment.specDomains());
    if (scriptTool == nullptr) {
        fprintf(stderr, "warning: could not find shell script tool\n");
        return nullptr;
    }

    return std::unique_ptr<ScriptResolver>(new ScriptResolver(scriptTool));
}

