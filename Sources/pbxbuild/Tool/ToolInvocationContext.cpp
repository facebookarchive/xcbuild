// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Tool/ToolInvocationContext.h>
#include <pbxbuild/Tool/SearchPaths.h>
#include <sstream>

using pbxbuild::Tool::ToolInvocationContext;
using ToolEnvironment = pbxbuild::Tool::ToolInvocationContext::ToolEnvironment;
using OptionsResult = pbxbuild::Tool::ToolInvocationContext::OptionsResult;
using CommandLineResult = pbxbuild::Tool::ToolInvocationContext::CommandLineResult;
using pbxbuild::ToolInvocation;
using pbxbuild::Tool::SearchPaths;
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
        // TODO(grp): AdditionalFlags
        // TODO(grp): ProductResourcesDir
        // TODO(grp): BitcodeArch
        // TODO(grp): StaticAnalyzerModeNameDescription
        // TODO(grp): DependencyInfoFile (from tool->dependencyInfoFile())
        // TOOD(grp): CommandProgressByType
        pbxsetting::Setting::Parse("DerivedFilesDir", environment.resolve("DERIVED_FILES_DIR")),
    };

    pbxsetting::Environment toolEnvironment = environment;
    toolEnvironment.insertFront(tool->defaultSettings(), true);
    toolEnvironment.insertFront(pbxsetting::Level(toolSettings), false);

    return ToolEnvironment(tool, toolEnvironment, inputs, outputs);
}

OptionsResult::
OptionsResult(std::vector<std::string> const &arguments, std::unordered_map<std::string, std::string> const &environment, std::vector<std::string> const &linkerArgs) :
    _arguments  (arguments),
    _environment(environment),
    _linkerArgs (linkerArgs)
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
    }), false);

    for (pbxsetting::Value const &arg : args) {
        std::string result = argEnvironment.expand(arg);
        arguments->push_back(result);
    }
}

static void
AddOptionArgumentValues(std::vector<std::string> *arguments, pbxsetting::Environment const &environment, std::string const &workingDirectory, std::vector<pbxsetting::Value> const &args, pbxspec::PBX::PropertyOption::shared_ptr const &option)
{
    if ((option->type() == "StringList" || option->type() == "stringlist") ||
        (option->type() == "PathList" || option->type() == "pathlist")) {
        std::vector<std::string> values = pbxsetting::Type::ParseList(environment.resolve(option->name()));
        if (option->flattenRecursiveSearchPathsInValue()) {
            values = SearchPaths::ExpandRecursive(environment, values, workingDirectory);
        }

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
Create(ToolEnvironment const &toolEnvironment, std::string const &workingDirectory, pbxspec::PBX::FileType::shared_ptr fileType, std::unordered_map<std::string, std::string> const &environmentVariables)
{
    pbxsetting::Environment const &environment = toolEnvironment.toolEnvironment();
    std::unordered_set<std::string> const &deletedProperties = toolEnvironment.tool()->deletedProperties();

    std::vector<std::string> arguments = { };
    std::unordered_map<std::string, std::string> toolEnvironmentVariables = environmentVariables;
    std::vector<std::string> linkerArgs = { };

    std::string architecture = environment.resolve("arch");

    for (pbxspec::PBX::PropertyOption::shared_ptr const &option : toolEnvironment.tool()->options()) {
        if (deletedProperties.find(option->name()) != deletedProperties.end()) {
            continue;
        }

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

        std::string flag;
        std::string value = environment.resolve(option->name());
        if (pbxsetting::Type::ParseBoolean(value)) {
            flag = option->commandLineFlag();
        } else {
            flag = option->commandLineFlagIfFalse();
        }
        if (!flag.empty()) {
            arguments.push_back(environment.expand(pbxsetting::Value::Parse(flag)));

            if (option->type() != "Boolean" && option->type() != "bool") {
                arguments.push_back(value);
            }
        }

        if (auto values = plist::CastTo <plist::Array> (option->values())) {
            for (size_t n = 0; n < values->count(); n++) {
                if (auto entry = values->value <plist::Dictionary> (n)) {
                    if (auto entryValue = entry->value <plist::String> ("Value")) {
                        if (entryValue->value() == value) {
                            if (auto entryFlag = entry->value <plist::String> ("CommandLineFlag")) {
                                arguments.push_back(environment.expand(pbxsetting::Value::Parse(entryFlag->value())));
                            }
                        }
                    }
                }
            }
        }

        if (option->hasCommandLinePrefixFlag()) {
            std::string const &prefix = option->commandLinePrefixFlag();
            pbxsetting::Value prefixValue = pbxsetting::Value::Parse(prefix) + pbxsetting::Value::Parse("$(value)");
            AddOptionArgumentValues(&arguments, environment, workingDirectory, { prefixValue }, option);
        }

        if (auto args = plist::CastTo <plist::Array> (option->commandLineArgs())) {
            std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
            AddOptionArgumentValues(&arguments, environment, workingDirectory, argsValues, option);
        } else if (auto argsValues = plist::CastTo <plist::Dictionary> (option->commandLineArgs())) {
            if (auto args = argsValues->value <plist::Array> (value)) {
                std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
                AddOptionArgumentValues(&arguments, environment, workingDirectory, argsValues, option);
            } else if (auto args = argsValues->value <plist::Array> ("<<otherwise>>")) {
                std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
                AddOptionArgumentValues(&arguments, environment, workingDirectory, argsValues, option);
            }
        }

        if (auto args = plist::CastTo <plist::Array> (option->additionalLinkerArgs())) {
            std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
            AddOptionArgumentValues(&linkerArgs, environment, workingDirectory, argsValues, option);
        } else if (auto argsValues = plist::CastTo <plist::Dictionary> (option->additionalLinkerArgs())) {
            if (auto args = argsValues->value <plist::Array> (value)) {
                std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
                AddOptionArgumentValues(&linkerArgs, environment, workingDirectory, argsValues, option);
            } else if (auto args = argsValues->value <plist::Array> ("<<otherwise>>")) {
                std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(args);
                AddOptionArgumentValues(&linkerArgs, environment, workingDirectory, argsValues, option);
            }
        }

        std::string const &variable = environment.expand(option->setValueInEnvironmentVariable());
        if (!variable.empty()) {
            toolEnvironmentVariables.insert({ variable, value });
        }

        // TODO(grp): Use PropertyOption::conditionFlavors().
        // TODO(grp): Use PropertyOption::isCommand{Input,Output}().
        // TODO(grp): Use PropertyOption::isInputDependency(), PropertyOption::outputDependencies(), etc..
    }

    return OptionsResult(arguments, toolEnvironmentVariables, linkerArgs);
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
Create(ToolEnvironment const &toolEnvironment, OptionsResult options, std::string const &executable, std::vector<std::string> const &specialArguments, std::unordered_set<std::string> const &removed)
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

    std::unordered_map<std::string, std::vector<std::string>> commandLineTokenValues = {
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
        if (removed.find(resolved) == removed.end()) {
            arguments.push_back(resolved);
        }
    }

    std::string invocationExecutable = (!arguments.empty() ? arguments.front() : "");
    std::vector<std::string> invocationArguments = std::vector<std::string>(arguments.begin() + (!arguments.empty() ? 1 : 0), arguments.end());

    return CommandLineResult(invocationExecutable, invocationArguments);
}

std::string ToolInvocationContext::
LogMessage(ToolEnvironment const &toolEnvironment)
{
    pbxsetting::Environment const &environment = toolEnvironment.toolEnvironment();
    std::string ruleName = environment.expand(toolEnvironment.tool()->ruleName());
    std::string ruleFormat = environment.expand(toolEnvironment.tool()->ruleFormat());

    if (!ruleName.empty()) {
        return ruleName;
    } else if (!ruleFormat.empty()) {
        return ruleFormat;
    } else {
        return std::string();
    }
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
    OptionsResult options = OptionsResult::Create(toolEnvironment, workingDirectory, nullptr);
    CommandLineResult commandLine = CommandLineResult::Create(toolEnvironment, options);
    std::string resolvedLogMessage = (!logMessage.empty() ? logMessage : ToolInvocationContext::LogMessage(toolEnvironment));
    return ToolInvocationContext::Create(toolEnvironment, options, commandLine, resolvedLogMessage, workingDirectory);
}
