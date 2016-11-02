/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_InfoPlistResolver_h
#define __pbxbuild_Tool_InfoPlistResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Tool/Input.h>

namespace pbxbuild {
namespace Tool {

class Context;

class InfoPlistResolver {
private:
    pbxspec::PBX::Tool::shared_ptr _tool;

public:
    explicit InfoPlistResolver(pbxspec::PBX::Tool::shared_ptr const &tool);
    ~InfoPlistResolver();

public:
    pbxspec::PBX::Tool::shared_ptr const &tool() const
    { return _tool; }

public:
    void resolve(
        Tool::Context *toolContext,
        pbxsetting::Environment const &environment,
        Tool::Input const &input) const;

public:
    static std::string ToolIdentifier()
    { return "com.apple.tools.info-plist-utility"; }

public:
    static std::unique_ptr<InfoPlistResolver>
    Create(Phase::Environment const &phaseEnvironment);
};

}
}

#endif // !__pbxbuild_Tool_InfoPlistResolver_h
