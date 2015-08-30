// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ToolInvocationContext_h
#define __pbxbuild_ToolInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class ToolInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit ToolInvocationContext(ToolInvocation const &invocation);
    ~ToolInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static ToolInvocationContext
    Create(
        pbxspec::PBX::Tool::shared_ptr const &tool,
        pbxsetting::Environment const &environment,
        std::vector<std::string> const &inputs,
        std::vector<std::string> const &outputs,
        std::string const &executable = "",
        std::vector<std::string> specialArguments = { }
    );
};

}

#endif // !__pbxbuild_ToolInvocationContext_h
