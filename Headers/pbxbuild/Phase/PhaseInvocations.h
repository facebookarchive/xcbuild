/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_PhaseInvocations_h
#define __pbxbuild_Phase_PhaseInvocations_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseEnvironment;

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
    Create(PhaseEnvironment const &phaseEnvironment, pbxproj::PBX::Target::shared_ptr const &target);
};

}
}

#endif // !__pbxbuild_Phase_PhaseInvocations_h
