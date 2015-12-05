/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>

using pbxbuild::Phase::LegacyTargetResolver;
using pbxbuild::Phase::PhaseEnvironment;
using libutil::FSUtil;

LegacyTargetResolver::
LegacyTargetResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

LegacyTargetResolver::
~LegacyTargetResolver()
{
}

std::unique_ptr<LegacyTargetResolver> LegacyTargetResolver::
Create(
    pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment,
    pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget
)
{
    pbxspec::PBX::Tool::shared_ptr scriptTool = phaseEnvironment.buildEnvironment().specManager()->tool("com.apple.commands.shell-script", phaseEnvironment.targetEnvironment().specDomains());
    if (scriptTool == nullptr) {
        fprintf(stderr, "warning: could not find shell script tool\n");
        return nullptr;
    }

    std::string const &workingDirectory = phaseEnvironment.targetEnvironment().workingDirectory();
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();

    auto context = pbxbuild::Tool::ScriptInvocationContext::Create(scriptTool, legacyTarget, environment, workingDirectory);
    std::vector<pbxbuild::ToolInvocation> invocations = { context.invocation() };

    return std::unique_ptr<LegacyTargetResolver>(new LegacyTargetResolver(invocations));
}
