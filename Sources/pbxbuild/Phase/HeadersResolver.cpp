// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/ScriptInvocationContext.h>

using pbxbuild::Phase::HeadersResolver;
using pbxbuild::Phase::PhaseContext;
using libutil::FSUtil;

HeadersResolver::
HeadersResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

HeadersResolver::
~HeadersResolver()
{
}

std::unique_ptr<HeadersResolver> HeadersResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::HeadersBuildPhase::shared_ptr const &buildPhase
)
{
    pbxspec::PBX::Tool::shared_ptr copyTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.compilers.pbxcp", phaseContext.targetEnvironment().specDomains());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    // TODO(grp): Create headers invocations.
    std::vector<pbxbuild::ToolInvocation> invocations = { };

    return std::make_unique<HeadersResolver>(invocations);
}
