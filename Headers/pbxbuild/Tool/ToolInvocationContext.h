// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ToolInvocationContext_h
#define __pbxbuild_ToolInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Tool {

class ToolEnvironment;
class OptionsResult;
class CommandLineResult;

class ToolInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit ToolInvocationContext(ToolInvocation const &invocation);
    ~ToolInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static std::string
    LogMessage(ToolEnvironment const &toolEnvironment);

public:
    static ToolInvocationContext
    Create(
        ToolEnvironment const &toolEnvironment,
        OptionsResult const &options,
        CommandLineResult const &commandLine,
        std::string const &logMessage,
        std::string const &workingDirectory,
        std::string const &dependencyInfo = "",
        std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles = { }
    );

public:
    static ToolInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &tool,
        std::vector<std::string> const &inputs,
        std::vector<std::string> const &outputs,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &logMessage = ""
    );
};

}
}

#endif // !__pbxbuild_ToolInvocationContext_h
