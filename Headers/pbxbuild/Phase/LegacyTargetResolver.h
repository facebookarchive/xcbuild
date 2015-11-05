/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
