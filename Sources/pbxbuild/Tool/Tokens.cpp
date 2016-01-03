/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/Tokens.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <sstream>

namespace Tool = pbxbuild::Tool;
using libutil::FSUtil;

Tool::Tokens::
Tokens(std::string const &executable, std::vector<std::string> const &arguments) :
    _executable(executable),
    _arguments (arguments)
{
}

Tool::Tokens::
~Tokens()
{
}

Tool::Tokens Tool::Tokens::
Create(
    std::string const &tokenized,
    std::string const &executable,
    std::vector<std::string> const &options,
    std::vector<std::string> const &specialArgs,
    std::vector<std::string> const &inputs,
    std::vector<std::string> const &outputs)
{
    std::vector<std::string> tokens;
    std::stringstream sstream = std::stringstream(tokenized);
    std::copy(std::istream_iterator<std::string>(sstream), std::istream_iterator<std::string>(), std::back_inserter(tokens));

    std::string input = (!inputs.empty() ? inputs.front() : "");
    std::string output = (!outputs.empty() ? outputs.front() : "");

    std::unordered_map<std::string, std::vector<std::string>> tokenValues = {
        { "input", { input } },
        { "output", { output } },
        { "inputs", inputs },
        { "outputs", outputs },
        { "options", options },
        { "exec-path", { executable } },
        { "special-args", specialArgs },
    };

    std::vector<std::string> arguments;
    for (std::string const &entry : tokens) {
        if (entry.find('[') == 0 && entry.find(']') == entry.size() - 1) {
            std::string token = entry.substr(1, entry.size() - 2);
            auto it = tokenValues.find(token);
            if (it != tokenValues.end()) {
                arguments.insert(arguments.end(), it->second.begin(), it->second.end());
                continue;
            }
        }

        arguments.push_back(entry);
    }

    std::string const &invocationExecutable = (!arguments.empty() ? arguments.front() : "");
    std::vector<std::string> invocationArguments = std::vector<std::string>(arguments.begin() + (!arguments.empty() ? 1 : 0), arguments.end());

    return Tool::Tokens(invocationExecutable, invocationArguments);
}

Tool::Tokens Tool::Tokens::
CommandLine(Tool::Environment const &toolEnvironment, Tool::OptionsResult options, std::string const &executable, std::vector<std::string> const &specialArguments)
{
    pbxspec::PBX::Tool::shared_ptr const &tool = toolEnvironment.tool();
    pbxsetting::Environment const &environment = toolEnvironment.environment();

    std::string toolCommandLine = environment.expand(tool->commandLine());
    std::string const &resolvedCommandLine = (!toolCommandLine.empty() ? toolCommandLine : "[exec-path] [options] [special-args]");

    std::string toolExecutable = environment.expand(tool->execPath());
    std::string const &resolvedExecutable = (!executable.empty() ? executable : toolExecutable);

    return Create(resolvedCommandLine, resolvedExecutable, options.arguments(), specialArguments, toolEnvironment.inputs(), toolEnvironment.outputs());
}

