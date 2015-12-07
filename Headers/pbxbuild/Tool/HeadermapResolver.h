/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_HeadermapResolver_h
#define __pbxbuild_HeadermapResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>

namespace pbxbuild {
namespace Tool {

class SearchPaths;
class HeadermapInfo;

class HeadermapResolver {
private:
    pbxspec::PBX::Tool::shared_ptr _tool;
    pbxspec::Manager::shared_ptr   _specManager;

public:
    HeadermapResolver(pbxspec::PBX::Tool::shared_ptr const &tool, pbxspec::Manager::shared_ptr const &specManager);

public:
    ToolInvocation invocation(
        pbxproj::PBX::Target::shared_ptr const &target,
        SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        HeadermapInfo *headermapInfo
    ) const;

public:
    static std::string ToolIdentifier()
    { return "com.apple.commands.built-in.headermap-generator"; }

public:
    static std::unique_ptr<HeadermapResolver>
    Create(Phase::PhaseEnvironment const &phaseEnvironment);
};

}
}

#endif // !__pbxbuild_HeadermapResolver_h
