// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/ToolInvocationContext.h>
#include <sstream>

using pbxbuild::ToolInvocationContext;
using ToolEnvironment = pbxbuild::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::ToolInvocationContext::CommandLineResult;
using pbxbuild::ToolInvocation;
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

ToolEnvironment::
ToolEnvironment(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &toolEnvironment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs) :
    _tool           (tool),
    _toolEnvironment(toolEnvironment),
    _inputs         (inputs),
    _outputs        (outputs)
{
}

ToolEnvironment::
~ToolEnvironment()
{
}

ToolEnvironment ToolEnvironment::
Create(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &environment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs)
{
    // TODO(grp); Match inputs with allowed tool input file types.

    std::string input = (!inputs.empty() ? inputs.front() : "");
    std::string output = (!outputs.empty() ? outputs.front() : "");

    std::vector<pbxsetting::Setting> toolSettings = {
        pbxsetting::Setting::Parse("InputPath", input),
        pbxsetting::Setting::Parse("InputFileName", FSUtil::GetBaseName(input)),
        pbxsetting::Setting::Parse("InputFileBase", FSUtil::GetBaseNameWithoutExtension(input)),
        pbxsetting::Setting::Parse("InputFileRelativePath", input), // TODO(grp): Relative.
        pbxsetting::Setting::Parse("InputFileBaseUniquefier", ""), // TODO(grp): Uniqueify.
        pbxsetting::Setting::Parse("OutputPath", output),
        pbxsetting::Setting::Parse("OutputFileName", FSUtil::GetBaseName(output)),
        pbxsetting::Setting::Parse("OutputFileBase", FSUtil::GetBaseNameWithoutExtension(output)),
        // TODO(grp): OutputDir (including tool->outputDir())
        // TODO(grp): AdditionalContentFilePaths
        // TODO(grp): ProductResourcesDir
        // TODO(grp): BitcodeArch
        // TODO(grp): StaticAnalyzerModeNameDescription
        // TODO(grp): DependencyInfoFile (from tool->dependencyInfoFile())
        // TOOD(grp): CommandProgressByType
        pbxsetting::Setting::Parse("DerivedFilesDir", environment.resolve("DERIVED_FILES_DIR")),
    };
    pbxsetting::Level toolLevel = pbxsetting::Level(toolSettings);

    std::vector<pbxsetting::Level> toolLevels = environment.assignment();
    toolLevels.insert(toolLevels.begin(), toolLevel);
    pbxsetting::Environment toolEnvironment = pbxsetting::Environment(toolLevels, toolLevels);

    return ToolEnvironment(tool, toolEnvironment, inputs, outputs);
}

OptionsResult::
OptionsResult(std::vector<std::string> const &arguments, std::map<std::string, std::string> const &environment) :
    _arguments  (arguments),
    _environment(environment)
{
}

OptionsResult::
~OptionsResult()
{
}

OptionsResult OptionsResult::
Create(ToolEnvironment const &toolEnvironment, std::map<std::string, std::string> const &environment)
{
    // TODO(grp): Parse each PropertyOption in the tool.
    std::vector<std::string> arguments = { };
    return OptionsResult(arguments, environment);
}

CommandLineResult::
CommandLineResult(std::string const &executable, std::vector<std::string> const &arguments) :
    _executable(executable),
    _arguments (arguments)
{
}

CommandLineResult::
~CommandLineResult()
{
}

CommandLineResult CommandLineResult::
Create(ToolEnvironment const &toolEnvironment, OptionsResult options, std::string const &executable, std::vector<std::string> const &specialArguments)
{
    pbxspec::PBX::Tool::shared_ptr tool = toolEnvironment.tool();

    std::string commandLineString = (!tool->commandLine().empty() ? tool->commandLine() : "[exec-path] [options] [special-args]");

    std::vector<std::string> commandLine;
    std::stringstream sstream = std::stringstream(commandLineString);
    std::copy(std::istream_iterator<std::string>(sstream), std::istream_iterator<std::string>(), std::back_inserter(commandLine));

    std::vector<std::string> inputs = toolEnvironment.inputs();
    std::vector<std::string> outputs = toolEnvironment.outputs();
    std::string input = (!inputs.empty() ? inputs.front() : "");
    std::string output = (!outputs.empty() ? outputs.front() : "");

    std::map<std::string, std::vector<std::string>> commandLineTokenValues = {
        { "input", { input } },
        { "output", { output } },
        { "inputs", inputs },
        { "outputs", outputs },
        { "options", options.arguments() },
        { "exec-path", { !executable.empty() ? executable : tool->execPath() } },
        { "special-args", specialArguments },
    };

    std::vector<std::string> arguments;
    for (std::string const &entry : commandLine) {
        if (entry.find('[') == 0 && entry.find(']') == entry.size() - 1) {
            std::string token = entry.substr(1, entry.size() - 2);
            auto it = commandLineTokenValues.find(token);
            if (it != commandLineTokenValues.end()) {
                arguments.insert(arguments.end(), it->second.begin(), it->second.end());
                continue;
            }
        }

        pbxsetting::Value value = pbxsetting::Value::Parse(entry);
        std::string resolved = toolEnvironment.toolEnvironment().expand(value);
        arguments.push_back(resolved);
    }

    std::string invocationExecutable = (!arguments.empty() ? arguments.front() : "");
    std::vector<std::string> invocationArguments = std::vector<std::string>(arguments.begin() + (!arguments.empty() ? 1 : 0), arguments.end());

    return CommandLineResult(invocationExecutable, invocationArguments);
}

std::string ToolInvocationContext::
LogMessage(ToolEnvironment const &toolEnvironment)
{
    return toolEnvironment.toolEnvironment().expand(toolEnvironment.tool()->ruleName());
}

ToolInvocationContext ToolInvocationContext::
Create(
    ToolEnvironment const &toolEnvironment,
    OptionsResult const &options,
    CommandLineResult const &commandLine,
    std::string const &logMessage,
    std::string const &workingDirectory,
    std::string const &dependencyInfo,
    std::string const &responsePath,
    std::string const &responseContents
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
        responsePath,
        responseContents,
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
    OptionsResult options = OptionsResult::Create(toolEnvironment);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options);
    std::string resolvedLogMessage = (!logMessage.empty() ? logMessage : ToolInvocationContext::LogMessage(toolEnvironment));
    return ToolInvocationContext::Create(toolEnvironment, options, commandLine, resolvedLogMessage, workingDirectory);
}
