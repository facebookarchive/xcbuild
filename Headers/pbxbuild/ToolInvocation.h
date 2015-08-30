// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ToolInvocation_h
#define __pbxbuild_ToolInvocation_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class ToolInvocation {
private:
    std::string                        _executable;
    std::vector<std::string>           _arguments;
    std::map<std::string, std::string> _environment;
    std::string                        _workingDirectory;

private:
    std::vector<std::string>           _inputs;
    std::vector<std::string>           _outputs;
    std::string                        _dependencyInfo;

private:
    std::string                        _logMessage;

public:
    ToolInvocation(
        std::string const                        &executable,
        std::vector<std::string> const           &arguments,
        std::map<std::string, std::string> const &environment,
        std::string const                        &workingDirectory,
        std::vector<std::string> const           &inputs,
        std::vector<std::string> const           &outputs,
        std::string const                        &dependencyInfo,
        std::string const                        &logMessage
    );
    ~ToolInvocation();

public:
    std::string const &executable(void) const
    { return _executable; }
    std::vector<std::string> const &arguments(void) const
    { return _arguments; }
    std::map<std::string, std::string> const &environment(void) const
    { return _environment; }
    std::string const &workingDirectory(void) const
    { return _workingDirectory; }

public:
    std::vector<std::string> const &inputs(void) const
    { return _inputs; }
    std::vector<std::string> const &outputs(void) const
    { return _outputs; }
    std::string const &dependencyInfo(void) const
    { return _dependencyInfo; }

public:
    std::string const &logMessage(void) const
    { return _logMessage; }
};

}

#endif // !__pbxbuild_ToolInvocation_h
