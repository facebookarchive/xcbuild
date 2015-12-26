/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_TouchResolver_h
#define __pbxbuild_TouchResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Phase/Environment.h>

namespace pbxbuild {
namespace Tool {

class Context;

class TouchResolver {
private:
    pbxspec::PBX::Tool::shared_ptr _tool;

public:
    explicit TouchResolver(pbxspec::PBX::Tool::shared_ptr const &tool);
    ~TouchResolver();

public:
    pbxspec::PBX::Tool::shared_ptr const &tool() const
    { return _tool; }

public:
    void resolve(
        Tool::Context *toolContext,
        pbxsetting::Environment const &environment,
        std::string const &input,
        std::vector<std::string> const &dependencies) const;

public:
    static std::string ToolIdentifier()
    { return "com.apple.tools.touch"; }

public:
    static std::unique_ptr<TouchResolver>
    Create(Phase::Environment const &phaseEnvironment);
};

}
}

#endif // !__pbxbuild_TouchResolver_h
