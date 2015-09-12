// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_HeadermapInvocationContext_h
#define __pbxbuild_HeadermapInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Tool {

class SearchPaths;

class HeadermapInvocationContext {
private:
    ToolInvocation           _invocation;
    std::vector<std::string> _systemHeadermapFiles;
    std::vector<std::string> _userHeadermapFiles;

public:
    HeadermapInvocationContext(ToolInvocation const &invocation, std::vector<std::string> const &systemHeadermapFiles, std::vector<std::string> const &userHeadermapFiles);
    ~HeadermapInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }
    std::vector<std::string> const &systemHeadermapFiles(void) const
    { return _systemHeadermapFiles; }
    std::vector<std::string> const &userHeadermapFiles(void) const
    { return _userHeadermapFiles; }

public:
    static HeadermapInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &headermapTool,
        pbxspec::Manager::shared_ptr const &specManager,
        pbxproj::PBX::Target::shared_ptr const &target,
        SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}
}

#endif // !__pbxbuild_HeadermapInvocationContext_h
