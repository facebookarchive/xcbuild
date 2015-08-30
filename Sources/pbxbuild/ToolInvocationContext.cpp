// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/ToolInvocationContext.h>
#include <sstream>

using pbxbuild::ToolInvocationContext;
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

static pbxsetting::Environment
CreateToolEnvironment(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &environment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs)
{
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
        // TODO(grp): pbxsetting::Setting::Parse("AdditionalContentFilePaths", ),
        // TODO(grp): pbxsetting::Setting::Parse("ProductResourcesDir", ),
        // TODO(grp): pbxsetting::Setting::Parse("BitcodeArch", ),
        pbxsetting::Setting::Parse("DerivedFilesDir", environment.resolve("DERIVED_FILES_DIR")),
    };
    pbxsetting::Level toolLevel = pbxsetting::Level(toolSettings);

    std::vector<pbxsetting::Level> toolLevels = environment.assignment();
    toolLevels.push_back(toolLevel);
    return pbxsetting::Environment(toolLevels, toolLevels);
}

static std::vector<std::string>
ExpandCommandLine(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &toolEnvironment, std::vector<std::string> options, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs, std::string const &executable, std::vector<std::string> const &specialArguments)
{
    std::string commandLineString = (!tool->commandLine().empty() ? tool->commandLine() : "[exec-path] [options] [special-args] [inputs] [outputs]");

    std::vector<std::string> commandLine;
    std::stringstream sstream = std::stringstream(commandLineString);
    std::copy(std::istream_iterator<std::string>(sstream), std::istream_iterator<std::string>(), std::back_inserter(commandLine));

    std::string input = (!inputs.empty() ? inputs.front() : "");
    std::string output = (!outputs.empty() ? outputs.front() : "");

    std::map<std::string, std::vector<std::string>> commandLineTokenValues = {
        { "input", { input } },
        { "output", { output } },
        { "inputs", inputs },
        { "outputs", outputs },
        { "options", options },
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
        std::string resolved = toolEnvironment.expand(value);
        arguments.push_back(resolved);
    }

    return arguments;
}

ToolInvocationContext ToolInvocationContext::
Create(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &environment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs, std::string const &executable, std::vector<std::string> specialArguments, std::string const &responsePath, std::string const &responseContents)
{
    // TODO(grp); Match inputs with allowed tool input file types.

    pbxsetting::Environment toolEnvironment = CreateToolEnvironment(tool, environment, inputs, outputs);

    std::vector<std::string> options; // TODO(grp): Expand options from tool.

    std::vector<std::string> arguments = ExpandCommandLine(tool, toolEnvironment, options, inputs, outputs, executable, specialArguments);

    std::string invocationExecutable = (!arguments.empty() ? arguments.front() : "");
    std::vector<std::string> invocationArguments = std::vector<std::string>(arguments.begin() + (!arguments.empty() ? 1 : 0), arguments.end());

    std::string ruleName = toolEnvironment.expand(tool->ruleName());

    pbxbuild::ToolInvocation invocation = pbxbuild::ToolInvocation(
        invocationExecutable,
        invocationArguments,
        { }, // TODO(grp): Environment variables.
        "", // TODO(grp): Working directory.
        inputs,
        outputs,
        "", // TODO(grp): Dependency info.
        responsePath,
        responseContents,
        ruleName
    );
    return ToolInvocationContext(invocation);
}
