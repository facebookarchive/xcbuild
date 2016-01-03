/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_Tokens_h
#define __pbxbuild_Tool_Tokens_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class Environment;
class OptionsResult;

/*
 * Evaluates tokens of the form [token], as found in tools.
 */
class Tokens {
private:
    std::string              _executable;
    std::vector<std::string> _arguments;

public:
    Tokens(std::string const &executable, std::vector<std::string> const &arguments);
    ~Tokens();

public:
    /*
     * The first argument.
     */
    std::string const &executable() const
    { return _executable; }

    /*
     * The remaining arguments after the first.
     */
    std::vector<std::string> const &arguments() const
    { return _arguments; }

public:
    /*
     * Evaluate tokens with the provided inputs.
     */
    static Tool::Tokens
    Create(
        std::string const &tokenized,
        std::string const &executable,
        std::vector<std::string> const &arguments,
        std::vector<std::string> const &specialArgs,
        std::vector<std::string> const &inputs,
        std::vector<std::string> const &outputs);

    /*
     * Evaluate tokens for a tool's command line.
     */
    static Tool::Tokens
    CommandLine(
        Tool::Environment const &toolEnvironment,
        Tool::OptionsResult options,
        std::string const &executable = "",
        std::vector<std::string> const &specialArguments = { });
};

}
}

#endif // !__pbxbuild_Tool_Tokens_h
