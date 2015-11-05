/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_CopyFilesResolver_h
#define __pbxbuild_Phase_CopyFilesResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class CopyFilesResolver {
private:
    std::vector<ToolInvocation> _invocations;

public:
    explicit CopyFilesResolver(std::vector<ToolInvocation> const &invocations);
    ~CopyFilesResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }

public:
    static std::unique_ptr<CopyFilesResolver>
    Create(
        PhaseContext const &phaseContext,
        pbxproj::PBX::CopyFilesBuildPhase::shared_ptr const &buildPhase
    );
};

}
}

#endif // !__pbxbuild_Phase_CopyFilesResolver_h
