// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ToolInvocationContext_h
#define __pbxbuild_ToolInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

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
    struct ToolEnvironment {
    private:
        pbxspec::PBX::Tool::shared_ptr _tool;
        pbxsetting::Environment        _toolEnvironment;
        std::vector<std::string>       _inputs;
        std::vector<std::string>       _outputs;

    public:
        ToolEnvironment(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &toolEnvironment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs);
        ~ToolEnvironment();

    public:
        pbxspec::PBX::Tool::shared_ptr const &tool() const
        { return _tool; }
        pbxsetting::Environment const &toolEnvironment() const
        { return _toolEnvironment; }
        std::vector<std::string> const &inputs() const
        { return _inputs; }
        std::vector<std::string> const &outputs() const
        { return _outputs; }

    public:
        static ToolEnvironment
        Create(pbxspec::PBX::Tool::shared_ptr const &tool, pbxsetting::Environment const &environment, std::vector<std::string> const &inputs, std::vector<std::string> const &outputs);
    };

public:
    struct OptionsResult {
    private:
        std::vector<std::string> _arguments;
        std::map<std::string, std::string> _environment;

    public:
        OptionsResult(std::vector<std::string> const &arguments, std::map<std::string, std::string> const &environment);
        ~OptionsResult();

    public:
        std::vector<std::string> const &arguments() const
        { return _arguments; }
        std::map<std::string, std::string> const &environment() const
        { return _environment; }

    public:
        static OptionsResult
        Create(ToolEnvironment const &toolEnvironment, std::map<std::string, std::string> const &environment = { });
    };

public:
    struct CommandLineResult {
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
        Create(ToolEnvironment const &toolEnvironment, OptionsResult options, std::string const &executable = "", std::vector<std::string> const &specialArguments = { });
    };

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
        std::string const &responsePath = "",
        std::string const &responseContents = ""
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

#endif // !__pbxbuild_ToolInvocationContext_h
