/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/CommandLineResult.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <sstream>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::CommandLineResult::
CommandLineResult(std::string const &executable, std::vector<std::string> const &arguments) :
    _executable(executable),
    _arguments (arguments)
{
}

Tool::CommandLineResult::
~CommandLineResult()
{
}

Tool::CommandLineResult Tool::CommandLineResult::
Create(Tool::Environment const &toolEnvironment, Tool::OptionsResult options, std::string const &executable, std::vector<std::string> const &specialArguments, std::unordered_set<std::string> const &removed)
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

    return Tool::CommandLineResult(invocationExecutable, invocationArguments);
}
