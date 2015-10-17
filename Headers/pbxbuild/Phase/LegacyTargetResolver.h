// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_LegacyTargetResolver_h
#define __pbxbuild_Phase_LegacyTargetResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class LegacyTargetResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit LegacyTargetResolver(std::vector<ToolInvocation> const &invocations);
    ~LegacyTargetResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<LegacyTargetResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget
    );
};

}
}

#endif // !__pbxbuild_Phase_LegacyTargetResolver_h
