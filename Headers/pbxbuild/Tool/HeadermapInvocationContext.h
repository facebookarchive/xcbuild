// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_HeadermapInvocationContext_h
#define __pbxbuild_HeadermapInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Tool {

class HeadermapInvocationContext {
private:
    std::vector<ToolInvocation::AuxiliaryFile> _auxiliaryFiles;

public:
    explicit HeadermapInvocationContext(std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles);
    ~HeadermapInvocationContext();

public:
    std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles(void) const
    { return _auxiliaryFiles; }

public:
    static HeadermapInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &headermapTool,
        pbxspec::PBX::Compiler::shared_ptr const &defaultCompiler,
        pbxspec::Manager::shared_ptr const &specManager,
        pbxproj::PBX::Target::shared_ptr const &target,
        pbxsetting::Environment const &environment
    );
};

}
}

#endif // !__pbxbuild_HeadermapInvocationContext_h
