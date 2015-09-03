// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/ScriptInvocationContext.h>
#include <pbxbuild/ToolInvocationContext.h>
#include <pbxbuild/FileTypeResolver.h>

using pbxbuild::ScriptInvocationContext;
using pbxbuild::ToolInvocationContext;
using ToolEnvironment = pbxbuild::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::ToolInvocationContext::CommandLineResult;
using pbxbuild::ToolInvocation;
using pbxbuild::FileTypeResolver;
using libutil::FSUtil;

ScriptInvocationContext::
ScriptInvocationContext(ToolInvocation const &invocation) :
    _invocation(invocation)
{
}

ScriptInvocationContext::
~ScriptInvocationContext()
{
}

ScriptInvocationContext ScriptInvocationContext::
Create(
    pbxspec::PBX::Tool::shared_ptr scriptTool,
    std::string const &script,
    std::string const &scriptPath,
    std::string const &shell,
    std::vector<pbxsetting::Value> const &inputFiles,
    std::vector<pbxsetting::Value> const &outputFiles,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory,
    std::string const &logMessageTitle
)
{
    std::unordered_map<std::string, std::string> values = environment.computeValues(pbxsetting::Condition::Empty());
    std::map<std::string, std::string> environmentVariables = std::map<std::string, std::string>(values.begin(), values.end());

    std::vector<std::string> inputs;
    std::transform(inputFiles.begin(), inputFiles.end(), std::back_inserter(inputs), [&](pbxsetting::Value const &input) -> std::string {
        return environment.expand(input);
    });

    std::vector<std::string> outputs;
    std::transform(outputFiles.begin(), outputFiles.end(), std::back_inserter(outputs), [&](pbxsetting::Value const &output) -> std::string {
        return environment.expand(output);
    });

    std::string scriptContents = "#!" + shell + "\n" + script;

    std::string logMessage = logMessageTitle + " " + scriptPath;

    ToolEnvironment toolEnvironment = ToolEnvironment::Create(scriptTool, environment, inputs, outputs);
    OptionsResult options = OptionsResult::Create(toolEnvironment, environmentVariables);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options, "/bin/sh", { "-c", scriptPath });
    ToolInvocationContext context = ToolInvocationContext::Create(toolEnvironment, options, commandLine, logMessage, workingDirectory, "", scriptPath, scriptContents);
    return ScriptInvocationContext(context.invocation());
}

ScriptInvocationContext ScriptInvocationContext::
Create(
    pbxspec::PBX::Tool::shared_ptr scriptTool,
    pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase,
    pbxsetting::Environment const &environment,
    std::string const &workingDirectory
)
{
    pbxsetting::Level level = pbxsetting::Level({
        pbxsetting::Setting::Parse("BuildPhaseName", (!buildPhase->name().empty() ? buildPhase->name() : "Run Script")),
        pbxsetting::Setting::Parse("BuildPhaseIdentifier", buildPhase->blueprintIdentifier()),
    });

    std::vector<pbxsetting::Level> levels = environment.assignment();
    levels.push_back(level);
    pbxsetting::Environment phaseEnvironment = pbxsetting::Environment(levels, levels);

    pbxsetting::Value scriptPath = pbxsetting::Value::Parse("$(TEMP_FILES_DIR)/Script-$(BuildPhaseIdentifier).sh");
    pbxsetting::Value title = pbxsetting::Value::Parse("PhaseScriptExecution $(BuildPhaseName:quote)");

    // TODO(grp): Use PBX::ShellScriptBuildPhase::showEnvVarsInLog().
    return Create(
        scriptTool,
        buildPhase->shellScript(),
        phaseEnvironment.expand(scriptPath),
        buildPhase->shellPath(),
        buildPhase->inputPaths(),
        buildPhase->outputPaths(),
        environment,
        workingDirectory,
        phaseEnvironment.expand(title)
    );
}

