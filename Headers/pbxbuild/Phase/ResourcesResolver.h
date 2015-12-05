/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_ResourcesResolver_h
#define __pbxbuild_Phase_ResourcesResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseEnvironment;

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
        PhaseEnvironment const &phaseEnvironment,
        pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_ResourcesResolver_h
