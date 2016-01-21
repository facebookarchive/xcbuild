/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_SwiftResolver_h
#define __pbxbuild_Tool_SwiftResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/File.h>
#include <pbxbuild/Tool/DittoResolver.h>

namespace pbxbuild {
namespace Tool {

class Context;

class SwiftResolver {
private:
    pbxspec::PBX::Compiler::shared_ptr _compiler;
    Tool::DittoResolver                _dittoResolver;

private:
    explicit SwiftResolver(pbxspec::PBX::Compiler::shared_ptr const &compiler, Tool::DittoResolver const &dittoResolver);

public:
    void resolve(
        Tool::Context *toolContext,
        pbxsetting::Environment const &environment,
        std::vector<Phase::File> const &input,
        std::string const &outputDirectory,
        bool isFramework) const;

public:
    static std::string ToolIdentifier()
    { return "com.apple.xcode.tools.swift.compiler"; }

public:
    static std::unique_ptr<SwiftResolver>
    Create(Phase::Environment const &phaseEnvironment, Tool::DittoResolver const &dittoResolver);
};

}
}

#endif // !__pbxbuild_Tool_SwiftResolver_h
