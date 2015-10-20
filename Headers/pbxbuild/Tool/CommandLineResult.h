// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_CommandLineResult_h
#define __pbxbuild_CommandLineResult_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class ToolEnvironment;
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
    Create(ToolEnvironment const &toolEnvironment, OptionsResult options, std::string const &executable = "", std::vector<std::string> const &specialArguments = { }, std::unordered_set<std::string> const &removedArguments = { });
};

}
}

#endif // !__pbxbuild_CommandLineResult_h
