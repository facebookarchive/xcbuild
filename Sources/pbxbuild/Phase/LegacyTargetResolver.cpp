/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/ScriptInvocationContext.h>

using pbxbuild::Phase::LegacyTargetResolver;
using pbxbuild::Phase::PhaseContext;
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
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget
)
{
    pbxspec::PBX::Tool::shared_ptr scriptTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.commands.shell-script", phaseContext.targetEnvironment().specDomains());
    if (scriptTool == nullptr) {
        fprintf(stderr, "warning: could not find shell script tool\n");
        return nullptr;
    }

    std::string const &workingDirectory = phaseContext.targetEnvironment().workingDirectory();
    pbxsetting::Environment const &environment = phaseContext.targetEnvironment().environment();

    auto context = pbxbuild::Tool::ScriptInvocationContext::Create(scriptTool, legacyTarget, environment, workingDirectory);
    std::vector<pbxbuild::ToolInvocation> invocations = { context.invocation() };

    return std::make_unique<LegacyTargetResolver>(invocations);
}
