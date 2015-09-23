// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_CopyInvocationContext_h
#define __pbxbuild_CopyInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Tool {

class CopyInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit CopyInvocationContext(ToolInvocation const &invocation);
    ~CopyInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static CopyInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &copyTool,
        std::string const &inputFile,
        std::string const &outputDirectory,
        std::string const &logMessageTitle,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}
}

#endif // !__pbxbuild_CopyInvocationContext_h
