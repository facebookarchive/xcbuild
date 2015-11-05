/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
        pbxspec::PBX::Tool::shared_ptr const &scriptTool,
        std::string const &shell,
        std::vector<std::string> const &arguments,
        std::unordered_map<std::string, std::string> const &environmentVariables,
        std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaries,
        std::vector<std::string> const &inputFiles,
        std::vector<std::string> const &outputFiles,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &logMessage
    );
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &scriptTool,
        pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &scriptTool,
        pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
    static ScriptInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &scriptTool,
        std::string const &inputFile,
        pbxbuild::TargetBuildRules::BuildRule::shared_ptr const &buildRule,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}
}

#endif // !__pbxbuild_ScriptInvocationContext_h
