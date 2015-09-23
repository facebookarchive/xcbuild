// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ScriptInvocationContext_h
#define __pbxbuild_ScriptInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/TargetBuildRules.h>

namespace pbxbuild {
namespace Tool {

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
        bool multipleInputs,
        std::vector<std::string> const &inputFiles,
        std::vector<std::string> const &outputFiles,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &logMessage
    );
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr scriptTool,
        pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr scriptTool,
        std::string const &inputFile,
        pbxbuild::TargetBuildRules::BuildRule::shared_ptr const &buildRule,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}
}

#endif // !__pbxbuild_ScriptInvocationContext_h
