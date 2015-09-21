// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/ScriptInvocationContext.h>

using pbxbuild::Phase::ResourcesResolver;
using pbxbuild::Phase::PhaseContext;
using libutil::FSUtil;

ResourcesResolver::
ResourcesResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

ResourcesResolver::
~ResourcesResolver()
{
}

std::unique_ptr<ResourcesResolver> ResourcesResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxspec::PBX::Tool::shared_ptr copyTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.compilers.pbxcp", phaseContext.targetEnvironment().specDomain());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    // TODO(grp): Create resources invocations.
    std::vector<pbxbuild::ToolInvocation> invocations = { };

    return std::make_unique<ResourcesResolver>(invocations);
}
