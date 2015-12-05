/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_FrameworksResolver_h
#define __pbxbuild_Phase_FrameworksResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseEnvironment;
class SourcesResolver;

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
        PhaseEnvironment const &phaseEnvironment,
        pbxproj::PBX::FrameworksBuildPhase::shared_ptr const &buildPhase,
        SourcesResolver const &sourcesResolver
    );
};

}
}

#endif // !__pbxbuild_Phase_FrameworksResolver_h
