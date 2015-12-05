/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ShellScriptResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>

using pbxbuild::Phase::ShellScriptResolver;
using pbxbuild::Phase::PhaseEnvironment;
using libutil::FSUtil;

ShellScriptResolver::
ShellScriptResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

ShellScriptResolver::
~ShellScriptResolver()
{
}

std::unique_ptr<ShellScriptResolver> ShellScriptResolver::
Create(
    pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment,
    pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase
)
{
    pbxspec::PBX::Tool::shared_ptr scriptTool = phaseEnvironment.buildEnvironment().specManager()->tool("com.apple.commands.shell-script", phaseEnvironment.targetEnvironment().specDomains());
    if (scriptTool == nullptr) {
        fprintf(stderr, "warning: could not find shell script tool\n");
        return nullptr;
    }

    std::string const &workingDirectory = phaseEnvironment.targetEnvironment().workingDirectory();
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();

    auto context = pbxbuild::Tool::ScriptInvocationContext::Create(scriptTool, buildPhase, environment, workingDirectory);
    std::vector<pbxbuild::ToolInvocation> invocations = { context.invocation() };

    return std::unique_ptr<ShellScriptResolver>(new ShellScriptResolver(invocations));
}
