/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_ToolResolver_h
#define __pbxbuild_ToolResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>

namespace pbxbuild {
namespace Tool {

class ToolContext;

class ToolResolver {
private:
    pbxspec::PBX::Tool::shared_ptr _tool;

public:
    explicit ToolResolver(pbxspec::PBX::Tool::shared_ptr const &tool);
    ~ToolResolver();

public:
    pbxspec::PBX::Tool::shared_ptr const &tool() const
    { return _tool; }

public:
    void resolve(
        ToolContext *toolContext,
        pbxsetting::Environment const &environment,
        std::vector<std::string> const &inputs,
        std::vector<std::string> const &outputs,
        std::string const &logMessage = ""
    ) const;

public:
    static std::unique_ptr<ToolResolver>
    Create(Phase::PhaseEnvironment const &phaseEnvironment, std::string const &identifier);
};

}
}

#endif // !__pbxbuild_ToolResolver_h
