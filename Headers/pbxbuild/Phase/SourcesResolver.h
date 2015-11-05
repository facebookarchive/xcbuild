/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_SourcesResolver_h
#define __pbxbuild_Phase_SourcesResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Phase {

class PhaseContext;

class SourcesResolver {
private:
    std::vector<ToolInvocation>                                                _invocations;
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> _variantArchitectureInvocations;
    std::string                                                                _linkerDriver;
    std::unordered_set<std::string>                                            _linkerArgs;

public:
    SourcesResolver(
        std::vector<ToolInvocation> const &invocations,
        std::map<std::pair<std::string, std::string>,
        std::vector<ToolInvocation>> const &variantArchitectureInvocations,
        std::string const &linkerDriver,
        std::unordered_set<std::string> const &linkerArgs
    );
    ~SourcesResolver();

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> const &variantArchitectureInvocations() const
    { return _variantArchitectureInvocations; }
    std::string const &linkerDriver() const
    { return _linkerDriver; }
    std::unordered_set<std::string> const &linkerArgs() const
    { return _linkerArgs; }

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
