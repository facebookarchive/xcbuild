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

    pbxsetting::Environment toolEnvironment = environment;
    toolEnvironment.insertFront(pbxsetting::Level(toolSettings));
    toolEnvironment.insertBack(tool->defaultSettings());

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

static bool
EvaluateCondition(std::string const &condition, pbxsetting::Environment const &environment)
{
    std::string expression = environment.expand(pbxsetting::Value::Parse(condition));

    // TODO(grp): Evaluate condition expression language correctly.
    if (expression.find("==") != std::string::npos) {
        return expression.substr(0, expression.find("==")) == expression.substr(expression.find("=="));
    }

    return true;
}

static void
AddOptionArgumentValue(std::vector<std::string> *arguments, pbxsetting::Environment const &environment, std::vector<pbxsetting::Value> const &args, std::string const &value)
{
    pbxsetting::Environment argEnvironment = environment;
    argEnvironment.insertFront(pbxsetting::Level({
        pbxsetting::Setting::Parse("value", value),
    }));

    for (pbxsetting::Value const &arg : args) {
        std::string result = argEnvironment.expand(arg);
        arguments->push_back(result);
    }
}

static void
AddOptionArgumentValues(std::vector<std::string> *arguments, pbxsetting::Environment const &environment, std::vector<pbxsetting::Value> const &args, pbxspec::PBX::PropertyOption::shared_ptr const &option)
{
    if ((option->type() == "StringList" || option->type() == "stringlist") ||
        (option->type() == "PathList" || option->type() == "pathlist")) {
        std::vector<std::string> values = environment.resolveList(option->name());
        for (std::string const &value : values) {
            AddOptionArgumentValue(arguments, environment, args, value);
        }
    } else {
        std::string value = environment.resolve(option->name());
        AddOptionArgumentValue(arguments, environment, args, value);
    }
}

static std::vector<pbxsetting::Value>
ArgumentValuesFromArray(plist::Array const *args)
{
    std::vector<pbxsetting::Value> values;
    for (size_t n = 0; n < args->count(); n++) {
        if (auto arg = args->value <plist::String> (n)) {
            values.push_back(pbxsetting::Value::Parse(arg->value()));
        }
    }
    return values;
}

OptionsResult OptionsResult::
Create(ToolEnvironment const &toolEnvironment, pbxspec::PBX::FileType::shared_ptr fileType, std::map<std::string, std::string> const &environmentVariables)
{
    // TODO(grp): Parse each PropertyOption in the tool.
    std::vector<std::string> arguments = { };

    pbxsetting::Environment const &environment = toolEnvironment.toolEnvironment();

    std::map<std::string, std::string> toolEnvironmentVariables = environmentVariables;

    std::string architecture = environment.resolve("arch");

    for (pbxspec::PBX::PropertyOption::shared_ptr const &option : toolEnvironment.tool()->options()) {
        if (!EvaluateCondition(option->condition(), environment)) {
            continue;
        }

        std::vector<std::string> const &architectures = option->architectures();
        if (!architectures.empty() && std::find(architectures.begin(), architectures.end(), architecture) == architectures.end()) {
            continue;
        }

        std::vector<std::string> const &fileTypes = option->fileTypes();
        if (!fileTypes.empty() && (fileType == nullptr || std::find(fileTypes.begin(), fileTypes.end(), fileType->identifier()) == fileTypes.end())) {
            continue;
        }

        pbxsetting::Environment optionEnvironment = environment;
        // TODO(grp): This should be inserted in front of the build system defaults.
        optionEnvironment.insertBack(pbxsetting::Level({
            option->defaultSetting(),
        }));

        std::string flag;
        std::string value = optionEnvironment.resolve(option->name());
        if (!value.empty() && value != "NO") {
            flag = option->commandLineFlag();
        } else {
            flag = option->commandLineFlagIfFalse();
        }
        if (!flag.empty()) {
            arguments.push_back(optionEnvironment.expand(pbxsetting::Value::Parse(flag)));

            if (option->type() != "Boolean" && option->type() != "bool") {
                arguments.push_back(value);
            }
        }

        // TODO(grp): Empty prefix should indicate each argument should be passed with no prefix.
        std::string const &prefix = option->commandLinePrefixFlag();
        if (!prefix.empty()) {
            pbxsetting::Value prefixValue = pbxsetting::Value::Parse(prefix) + pbxsetting::Value::Parse("$(value)");
            AddOptionArgumentValues(&arguments, optionEnvironment, { prefixValue }, option);
        }

        if (auto args = plist::CastTo <plist::Array> (option->commandLineArgs())) {
            std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
            AddOptionArgumentValues(&arguments, optionEnvironment, argsValues, option);
        } else if (auto argsValues = plist::CastTo <plist::Dictionary> (option->commandLineArgs())) {
            if (auto args = argsValues->value <plist::Array> (value)) {
                std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
                AddOptionArgumentValues(&arguments, optionEnvironment, argsValues, option);
            } else if (auto args = argsValues->value <plist::Array> ("<<otherwise>>")) {
                std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
                AddOptionArgumentValues(&arguments, optionEnvironment, argsValues, option);
            }
        }

        std::string const &variable = option->setValueInEnvironmentVariable();
        if (!variable.empty()) {
            toolEnvironmentVariables.insert({ variable, value });
        }

        // TODO(grp): Use PropertyOption::conditionFlavors().
        // TODO(grp): Use PropertyOption::flattenRecursiveSearchPathsInValue().
        // TODO(grp): Use PropertyOption::isCommand{Input,Output}().
        // TODO(grp): Use PropertyOption::isInputDependency(), PropertyOption::outputDependencies(), etc..
    }

    return OptionsResult(arguments, toolEnvironmentVariables);
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
    std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles
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
        auxiliaryFiles,
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
    OptionsResult options = OptionsResult::Create(toolEnvironment, nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options);
    std::string resolvedLogMessage = (!logMessage.empty() ? logMessage : ToolInvocationContext::LogMessage(toolEnvironment));
    return ToolInvocationContext::Create(toolEnvironment, options, commandLine, resolvedLogMessage, workingDirectory);
}
