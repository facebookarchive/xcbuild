/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_CommandLineResult_h
#define __pbxbuild_CommandLineResult_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class Environment;
class OptionsResult;

class CommandLineResult {
private:
    std::string _executable;
    std::vector<std::string> _arguments;

public:
    CommandLineResult(std::string const &executable, std::vector<std::string> const &arguments);
    ~CommandLineResult();

public:
    std::string const &executable() const
    { return _executable; }
    std::vector<std::string> const &arguments() const
    { return _arguments; }

public:
    static CommandLineResult
    Create(Tool::Environment const &toolEnvironment, OptionsResult options, std::string const &executable = "", std::vector<std::string> const &specialArguments = { }, std::unordered_set<std::string> const &removedArguments = { });
};

}
}

#endif // !__pbxbuild_CommandLineResult_h
