// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_Phase_PhaseInvocations_h
#define __pbxbuild_Phase_PhaseInvocations_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class PhaseInvocations {
private:
    std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<ToolInvocation>> _invocations;

public:
    PhaseInvocations(std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<ToolInvocation>> const &invocations);
    ~PhaseInvocations();

public:
    std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<ToolInvocation>> const &invocations() const
    { return _invocations; }

public:
    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> orderedPhases(void);

public:
    std::vector<ToolInvocation> phaseInvocations(pbxproj::PBX::BuildPhase::shared_ptr const &phase);

public:
    static PhaseInvocations
    Create(PhaseContext const &phaseContext, pbxproj::PBX::Target::shared_ptr const &target);
};

}
}

#endif // !__pbxbuild_Phase_PhaseInvocations_h
