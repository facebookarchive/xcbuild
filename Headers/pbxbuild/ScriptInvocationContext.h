// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ScriptInvocationContext_h
#define __pbxbuild_ScriptInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class ScriptInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit ScriptInvocationContext(ToolInvocation const &invocation);
    ~ScriptInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr scriptTool,
        std::string const &script,
        std::string const &scriptPath,
        std::string const &shell,
        std::vector<pbxsetting::Value> const &inputFiles,
        std::vector<pbxsetting::Value> const &outputFiles,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &logMessageTitle
    );
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr scriptTool,
        pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}

#endif // !__pbxbuild_ScriptInvocationContext_h
