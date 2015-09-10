// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_CompilerInvocationContext_h
#define __pbxbuild_CompilerInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class TypeResolvedFile;

class CompilerInvocationContext {
private:
    ToolInvocation _invocation;

public:
    explicit CompilerInvocationContext(ToolInvocation const &invocation);
    ~CompilerInvocationContext();

public:
    ToolInvocation const &invocation(void) const
    { return _invocation; }

public:
    static CompilerInvocationContext
    Create(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        TypeResolvedFile const &input,
        std::vector<std::string> const &inputArguments,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}

#endif // !__pbxbuild_CompilerInvocationContext_h
