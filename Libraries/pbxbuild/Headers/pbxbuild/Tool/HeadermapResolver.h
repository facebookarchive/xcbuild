/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_HeadermapResolver_h
#define __pbxbuild_Tool_HeadermapResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Tool/Invocation.h>
#include <pbxbuild/Phase/Environment.h>

namespace pbxbuild {
namespace Tool {

class SearchPaths;
class Context;

class HeadermapResolver {
private:
    pbxspec::PBX::Tool::shared_ptr     _tool;
    pbxspec::PBX::Compiler::shared_ptr _compiler;
    pbxspec::Manager::shared_ptr       _specManager;

public:
    HeadermapResolver(pbxspec::PBX::Tool::shared_ptr const &tool, pbxspec::PBX::Compiler::shared_ptr const &compiler, pbxspec::Manager::shared_ptr const &specManager);

public:
    void resolve(
        Tool::Context *toolContext,
        pbxsetting::Environment const &environment,
        pbxproj::PBX::Target::shared_ptr const &target) const;

public:
    pbxspec::PBX::Tool::shared_ptr const &tool() const
    { return _tool; }

public:
    static std::string ToolIdentifier()
    { return "com.apple.commands.built-in.headermap-generator"; }

public:
    static std::unique_ptr<HeadermapResolver>
    Create(Phase::Environment const &phaseEnvironment, pbxspec::PBX::Compiler::shared_ptr const &compiler);
};

}
}

#endif // !__pbxbuild_Tool_HeadermapResolver_h
