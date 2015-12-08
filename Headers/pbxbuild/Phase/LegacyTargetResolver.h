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

namespace Tool { class ToolContext; }

namespace Phase {

class PhaseEnvironment;

class LegacyTargetResolver {
private:
    pbxproj::PBX::LegacyTarget::shared_ptr _legacyTarget;

public:
    explicit LegacyTargetResolver(pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget);
    ~LegacyTargetResolver();

public:
    pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget() const
    { return _legacyTarget; }

public:
    bool resolve(PhaseEnvironment const &phaseEnvironment, Tool::ToolContext *toolContext);
};

}
}

#endif // !__pbxbuild_Phase_LegacyTargetResolver_h
