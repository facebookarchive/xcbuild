/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_SymlinkResolver_h
#define __pbxbuild_SymlinkResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Phase/Environment.h>

namespace pbxbuild {
namespace Tool {

class Context;

class SymlinkResolver {
private:
    pbxspec::PBX::Tool::shared_ptr _tool;

private:
    explicit SymlinkResolver(pbxspec::PBX::Tool::shared_ptr const &tool);

public:
    void resolve(
        Tool::Context *toolContext,
        std::string const &workingDirectory,
        std::string const &symlinkPath,
        std::string const &targetPath,
        bool productStructure = false) const;

public:
    static std::string ToolIdentifier()
    { return "com.apple.tools.symlink"; }

public:
    static std::unique_ptr<SymlinkResolver>
    Create(Phase::Environment const &phaseEnvironment);
};

}
}

#endif // !__pbxbuild_SymlinkResolver_h
