// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Build/DefaultFormatter.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/BuildContext.h>

using pbxbuild::Build::DefaultFormatter;
using pbxbuild::ToolInvocation;

DefaultFormatter::
DefaultFormatter(bool color) :
    Formatter(),
    _color   (color)
{
}

DefaultFormatter::
~DefaultFormatter()
{
}

#define ANSI_STYLE_BOLD    std::string(_color ? "\033[1m"  : "")
#define ANSI_STYLE_NO_BOLD std::string(_color ? "\033[22m" : "")
#define ANSI_COLOR_RED     std::string(_color ? "\x1b[31m" : "")
#define ANSI_COLOR_GREEN   std::string(_color ? "\x1b[32m" : "")
#define ANSI_COLOR_CYAN    std::string(_color ? "\x1b[36m" : "")
#define ANSI_COLOR_RESET   std::string(_color ? "\x1b[0m"  : "")

#define INDENT std::string("    ")

static std::string
FormatInvocation(ToolInvocation const &invocation, bool _color)
{
    std::string message = invocation.logMessage();
    std::string::size_type space = message.find(' ');
    if (space != std::string::npos) {
        message = ANSI_STYLE_BOLD + message.substr(0, space) + ANSI_STYLE_NO_BOLD + message.substr(space);
    }
    return message;
}

static std::string
FormatAction(std::string const &action)
{
    std::string result = action;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string DefaultFormatter::
begin(BuildContext const &buildContext)
{
    std::string result;

    pbxsetting::Environment environment = pbxsetting::Environment::Empty();
    for (pbxsetting::Level const &level : buildContext.overrideLevels()) {
        environment.insertFront(level, false);
    }

    std::unordered_map<std::string, std::string> values = environment.computeValues(pbxsetting::Condition::Empty());
    if (!values.empty()) {
        std::map<std::string, std::string> orderedValues = std::map<std::string, std::string>(values.begin(), values.end());

        result += "Build settings from the command line:\n";
        for (auto const &entry : orderedValues) {
            result += INDENT + entry.first + "=" + entry.second + "\n";
        }
        result += "\n";
    }

    return result;
}

std::string DefaultFormatter::
success(BuildContext const &buildContext)
{
    std::string result;

    result += "\n" + ANSI_STYLE_BOLD + ANSI_COLOR_GREEN;
    result += "** " + FormatAction(buildContext.action()) + " SUCCEEDED **";
    result += ANSI_STYLE_NO_BOLD + ANSI_COLOR_RESET + "\n";

    return result;
}

std::string DefaultFormatter::
failure(BuildContext const &buildContext, std::vector<ToolInvocation> const &failingInvocations)
{
    std::string result;

    result += "\n" + ANSI_STYLE_BOLD + ANSI_COLOR_RED;
    result += "** " + FormatAction(buildContext.action()) + " FAILED **";
    result += ANSI_STYLE_NO_BOLD + ANSI_COLOR_RESET + "\n";

    result += "\nThe following build commands failed:\n";
    for (pbxbuild::ToolInvocation const &invocation : failingInvocations) {
        result += INDENT + FormatInvocation(invocation, _color) + "\n";
    }
    result += "(" + std::to_string(failingInvocations.size()) + " failure" + (failingInvocations.size() != 1 ? "s" : "") + ")\n";

    return result;
}

std::string DefaultFormatter::
beginTarget(BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::string result;
    result += "\n" + ANSI_STYLE_BOLD + ANSI_COLOR_CYAN;
    result += "=== ";

    result += FormatAction(buildContext.action()) + " ";
    result += std::string(target->type() == pbxproj::PBX::Target::kTypeLegacy ? "LEGACY TARGET" : "TARGET") + " ";
    result += target->name() + " ";
    result += "OF PROJECT " + target->project()->name() + " ";
    if (buildContext.defaultConfiguration()) {
        result += "WITH THE DEFAULT CONFIGURATION (" + buildContext.configuration() + ")";
    } else {
        result += "WITH CONFIGURATION " + buildContext.configuration();
    }

    result += " ===";
    result += ANSI_STYLE_NO_BOLD + ANSI_COLOR_RESET + "\n";
    return result;
}

std::string DefaultFormatter::
checkDependencies(pbxproj::PBX::Target::shared_ptr const &target)
{
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        return "\nCheck dependencies\n";
    } else {
        return std::string();
    }
}

std::string DefaultFormatter::
beginWriteAuxiliaryFiles(pbxproj::PBX::Target::shared_ptr const &target)
{
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        return "\nWrite auxiliary files\n";
    } else {
        return std::string();
    }
}

std::string DefaultFormatter::
createAuxiliaryDirectory(std::string const &directory)
{
    return "/bin/mkdir -p " + directory + "\n";
}

std::string DefaultFormatter::
writeAuxiliaryFile(std::string const &file)
{
    return "write-file " + file + "\n";
}

std::string DefaultFormatter::
setAuxiliaryExecutable(std::string const &file)
{
    return "chmod 0755 " + file + "\n";
}

std::string DefaultFormatter::
finishWriteAuxiliaryFiles(pbxproj::PBX::Target::shared_ptr const &target)
{
    return "";
}

std::string DefaultFormatter::
createProductStructure(pbxproj::PBX::Target::shared_ptr const &target)
{
    if (target->type() == pbxproj::PBX::Target::kTypeNative) {
        // TODO(grp): Implement this fully.
        return "\nCreate product structure\n";
    } else {
        return std::string();
    }
}

std::string DefaultFormatter::
invocation(ToolInvocation const &invocation, std::string const &executable)
{
    std::string message;
    message += "\n";

    message += FormatInvocation(invocation, _color) + "\n";
    message += INDENT + "cd " + invocation.workingDirectory() + "\n";

    std::map<std::string, std::string> sortedEnvironment = std::map<std::string, std::string>(invocation.environment().begin(), invocation.environment().end());
    for (std::pair<std::string, std::string> const &entry : sortedEnvironment) {
        message += INDENT + "export " + entry.first + "=" + entry.second + "\n";
    }

    message += INDENT + executable;
    for (std::string const &arg : invocation.arguments()) {
        message += " " + arg;
    }
    message += "\n";

    return message;
}

std::shared_ptr<DefaultFormatter> DefaultFormatter::
Create(bool color)
{
    return std::make_shared<DefaultFormatter>(color);
}
