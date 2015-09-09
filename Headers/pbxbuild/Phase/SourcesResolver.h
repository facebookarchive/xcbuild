// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_SourcesResolver_h
#define __pbxbuild_Phase_SourcesResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class SourcesResolver {
private:
    std::vector<ToolInvocation> _invocations;
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> _variantArchitectureInvocations;

public:
    SourcesResolver(std::vector<ToolInvocation> const &invocations, std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> const &variantArchitectureInvocations);
    ~SourcesResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> const &variantArchitectureInvocations() const
    { return _variantArchitectureInvocations; }

public:
    static std::unique_ptr<SourcesResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::SourcesBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_SourcesResolver_h
