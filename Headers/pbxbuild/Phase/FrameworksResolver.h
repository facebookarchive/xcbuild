// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_FrameworksResolver_h
#define __pbxbuild_Phase_FrameworksResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class FrameworksResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit FrameworksResolver(std::vector<ToolInvocation> const &invocations);
    ~FrameworksResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<FrameworksResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::FrameworksBuildPhase::shared_ptr const &buildPhase,
        std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> const &sourcesInvocations
    );
};

}
}

#endif // !__pbxbuild_Phase_FrameworksResolver_h
