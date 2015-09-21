// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_HeadersResolver_h
#define __pbxbuild_Phase_HeadersResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class HeadersResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit HeadersResolver(std::vector<ToolInvocation> const &invocations);
    ~HeadersResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<HeadersResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::HeadersBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_HeadersResolver_h
