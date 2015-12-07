/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_CompilerInvocationContext_h
#define __pbxbuild_CompilerInvocationContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>

namespace pbxbuild {

class TypeResolvedFile;

namespace Tool {

class SearchPaths;
class HeadermapInfo;
class PrecompiledHeaderInfo;

class CompilerInvocationContext {
private:
    ToolInvocation                         _invocation;
    std::shared_ptr<PrecompiledHeaderInfo> _precompiledHeaderInfo;
    std::vector<std::string>               _linkerArgs;

public:
    CompilerInvocationContext(ToolInvocation const &invocation, std::shared_ptr<PrecompiledHeaderInfo> const &precompiledHeaderInfo, std::vector<std::string> const &linkerArgs);
    ~CompilerInvocationContext();

public:
    ToolInvocation const &invocation() const
    { return _invocation; }
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
        HeadermapInfo const &headermapInfo,
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
