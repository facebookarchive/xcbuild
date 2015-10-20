// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_OptionsResult_h
#define __pbxbuild_OptionsResult_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class ToolEnvironment;

class OptionsResult {
private:
    std::vector<std::string>                     _arguments;
    std::unordered_map<std::string, std::string> _environment;
    std::vector<std::string>                     _linkerArgs;

public:
    OptionsResult(std::vector<std::string> const &arguments, std::unordered_map<std::string, std::string> const &environment, std::vector<std::string> const &linkerArgs);
    ~OptionsResult();

public:
    std::vector<std::string> const &arguments() const
    { return _arguments; }
    std::unordered_map<std::string, std::string> const &environment() const
    { return _environment; }
    std::vector<std::string> const &linkerArgs() const
    { return _linkerArgs; }

public:
    static OptionsResult
    Create(ToolEnvironment const &toolEnvironment, std::string const &workingDirectory, pbxspec::PBX::FileType::shared_ptr fileType, std::unordered_map<std::string, std::string> const &environment = { });
};

}
}

#endif // !__pbxbuild_OptionsResult_h
