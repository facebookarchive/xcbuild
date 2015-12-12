/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/ToolEnvironment.h>
#include <pbxbuild/Tool/SearchPaths.h>

using pbxbuild::Tool::OptionsResult;
using pbxbuild::Tool::ToolEnvironment;
using pbxbuild::Tool::SearchPaths;
using libutil::FSUtil;

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

static void
AddOptionValuesArguments(std::vector<std::string> *arguments, pbxsetting::Environment const &environment, std::string const &workingDirectory, plist::Array const *values, std::string const &value, pbxspec::PBX::PropertyOption::shared_ptr const &option)
{
    if (values == nullptr) {
        return;
    }

    for (size_t n = 0; n < values->count(); n++) {
        if (auto entry = values->value <plist::Dictionary> (n)) {
            if (auto entryValue = entry->value <plist::String> ("Value")) {
                if (entryValue->value() == value) {
                    if (auto entryFlag = entry->value <plist::String> ("CommandLineFlag")) {
                        arguments->push_back(environment.expand(pbxsetting::Value::Parse(entryFlag->value())));
                    } else if (auto entryArgs = entry->value <plist::Array> ("CommandLineArgs")) {
                        std::vector<pbxsetting::Value> argsValues = ArgumentValuesFromArray(entryArgs);
                        AddOptionArgumentValues(arguments, environment, workingDirectory, argsValues, option);
                    }
                }
            }
        }
    }
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

        std::string value = environment.resolve(option->name());

        if (option->type() == "Boolean" || option->type() == "bool") {
            std::string flag;

            if (pbxsetting::Type::ParseBoolean(value)) {
                flag = option->commandLineFlag();
            } else {
                flag = option->commandLineFlagIfFalse();
            }

            if (!flag.empty()) {
                /* Boolean flags don't get the flag value after, since that would be just YES or NO. */
                arguments.push_back(environment.expand(pbxsetting::Value::Parse(flag)));
            }
        } else {
            if (!value.empty()) {
                /* Pass both the command line flag and the option value itself. */
                std::string const &flag = option->commandLineFlag();
                if (!flag.empty()) {
                    arguments.push_back(environment.expand(pbxsetting::Value::Parse(flag)));
                    arguments.push_back(value);
                }
            }
        }

        AddOptionValuesArguments(&arguments, environment, workingDirectory, plist::CastTo<plist::Array>(option->values()), value, option);
        AddOptionValuesArguments(&arguments, environment, workingDirectory, plist::CastTo<plist::Array>(option->allowedValues()), value, option);

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

