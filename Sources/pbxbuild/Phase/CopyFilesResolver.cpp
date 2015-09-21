// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/ScriptInvocationContext.h>

using pbxbuild::Phase::CopyFilesResolver;
using pbxbuild::Phase::PhaseContext;
using libutil::FSUtil;

CopyFilesResolver::
CopyFilesResolver(std::vector<pbxbuild::ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

CopyFilesResolver::
~CopyFilesResolver()
{
}

std::unique_ptr<CopyFilesResolver> CopyFilesResolver::
Create(
    pbxbuild::Phase::PhaseContext const &phaseContext,
    pbxproj::PBX::CopyFilesBuildPhase::shared_ptr const &buildPhase
)
{
    pbxspec::PBX::Tool::shared_ptr copyTool = phaseContext.buildEnvironment().specManager()->tool("com.apple.compilers.pbxcp", phaseContext.targetEnvironment().specDomain());
    if (copyTool == nullptr) {
        fprintf(stderr, "warning: could not find copy tool\n");
        return nullptr;
    }

    // TODO(grp): Create copy file invocations.
    std::vector<pbxbuild::ToolInvocation> invocations = { };

    return std::make_unique<CopyFilesResolver>(invocations);
}
