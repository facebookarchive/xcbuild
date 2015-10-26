// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_CompilerInvocationContext_h
#define __pbxbuild_CompilerInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>

namespace pbxbuild {

class TypeResolvedFile;

namespace Tool {

class SearchPaths;
class HeadermapInvocationContext;
class PrecompiledHeaderInfo;

class CompilerInvocationContext {
private:
    ToolInvocation                         _invocation;
    std::string                            _output;
    std::shared_ptr<PrecompiledHeaderInfo> _precompiledHeaderInfo;
    std::vector<std::string>               _linkerArgs;

public:
    CompilerInvocationContext(ToolInvocation const &invocation, std::string const &output, std::shared_ptr<PrecompiledHeaderInfo> const &precompiledHeaderInfo, std::vector<std::string> const &linkerArgs);
    ~CompilerInvocationContext();

public:
    ToolInvocation const &invocation() const
    { return _invocation; }
    std::string const &output() const
    { return _output; }
    std::shared_ptr<PrecompiledHeaderInfo> const &precompiledHeaderInfo() const
    { return _precompiledHeaderInfo; }
    std::vector<std::string> const &linkerArgs() const
    { return _linkerArgs; }

public:
    static CompilerInvocationContext
    CreateSource(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        TypeResolvedFile const &input,
        std::vector<std::string> const &inputArguments,
        std::string const &outputBaseName,
        HeadermapInvocationContext const &headermaps,
        SearchPaths const &searchPaths,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );

    static CompilerInvocationContext
    CreatePrecompiledHeader(
        pbxspec::PBX::Compiler::shared_ptr const &compiler,
        PrecompiledHeaderInfo const &precompiledHeaderInfo,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    );
};

}
}

#endif // !__pbxbuild_CompilerInvocationContext_h
