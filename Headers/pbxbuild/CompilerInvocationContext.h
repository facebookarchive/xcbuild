// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_CompilerInvocationContext_h
#define __pbxbuild_CompilerInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class TypeResolvedFile;
namespace Tool { class HeadermapInvocationContext; }
namespace Tool { class SearchPaths; }

class CompilerInvocationContext {
private:
    ToolInvocation           _invocation;
    std::vector<std::string> _linkerArgs;

public:
    explicit CompilerInvocationContext(ToolInvocation const &invocation, std::vector<std::string> const &linkerArgs);
    ~CompilerInvocationContext();

public:
    ToolInvocation const &invocation() const
    { return _invocation; }
    std::vector<std::string> const &linkerArgs() const
    { return _linkerArgs; }

public:
    static CompilerInvocationContext
    Create(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        TypeResolvedFile const &input,
        std::vector<std::string> const &inputArguments,
        Tool::HeadermapInvocationContext const &headermaps,
        Tool::SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}

#endif // !__pbxbuild_CompilerInvocationContext_h
