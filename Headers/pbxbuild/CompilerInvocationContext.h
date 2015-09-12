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
    std::string              _output;
    std::vector<std::string> _linkerArgs;

public:
    CompilerInvocationContext(ToolInvocation const &invocation, std::string const &output, std::vector<std::string> const &linkerArgs);
    ~CompilerInvocationContext();

public:
    ToolInvocation const &invocation() const
    { return _invocation; }
    std::string const &output() const
    { return _output; }
    std::vector<std::string> const &linkerArgs() const
    { return _linkerArgs; }

public:
    static CompilerInvocationContext
    CreateSource(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        TypeResolvedFile const &input,
        std::vector<std::string> const &inputArguments,
        std::string const &outputBaseName,
        std::string const &prefixHeader,
        Tool::HeadermapInvocationContext const &headermaps,
        Tool::SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );

    static CompilerInvocationContext
    CreatePrecompiledHeader(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        std::string const &input,
        pbxspec::PBX::FileType::shared_ptr const &fileType,
        Tool::HeadermapInvocationContext const &headermaps,
        Tool::SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );

    static CompilerInvocationContext
    Create(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        std::string const &input,
        pbxspec::PBX::FileType::shared_ptr const &fileType,
        std::string const &dialectSuffix,
        std::vector<std::string> const &inputArguments,
        std::string const &output,
        std::string const &prefixHeader,
        Tool::HeadermapInvocationContext const &headermaps,
        Tool::SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory,
        std::string const &logTitle
    );
};

}

#endif // !__pbxbuild_CompilerInvocationContext_h
