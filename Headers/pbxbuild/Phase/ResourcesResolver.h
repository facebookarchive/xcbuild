// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_ResourcesResolver_h
#define __pbxbuild_Phase_ResourcesResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class ResourcesResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit ResourcesResolver(std::vector<ToolInvocation> const &invocations);
    ~ResourcesResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<ResourcesResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_ResourcesResolver_h
