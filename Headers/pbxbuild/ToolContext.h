// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_ToolContext_h
#define __pbxbuild_ToolContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class ToolContext {
private:
    ToolInvocation _invocation;

public:
    explicit ToolContext(ToolInvocation const &invocation);
    ~ToolContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static ToolContext
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

#endif // !__pbxbuild_ToolContext_h
