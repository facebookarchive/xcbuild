// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/ShellScriptResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>

using pbxbuild::Phase::ShellScriptResolver;
using pbxbuild::Phase::PhaseContext;
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
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase
)
{
    pbxspec::PBX::Tool::shared_ptr scriptTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.commands.shell-script", phaseContext.targetEnvironment().specDomains());
    if (scriptTool == nullptr) {
        fprintf(stderr, "warning: could not find shell script tool\n");
        return nullptr;
    }

    std::string workingDirectory = phaseContext.targetEnvironment().workingDirectory();
    auto context = pbxbuild::Tool::ScriptInvocationContext::Create(scriptTool, buildPhase, phaseContext.targetEnvironment().environment(), workingDirectory);
    std::vector<pbxbuild::ToolInvocation> invocations = { context.invocation() };

    return std::make_unique<ShellScriptResolver>(invocations);
}
