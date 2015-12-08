/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_ClangResolver_h
#define __pbxbuild_ClangResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>

namespace pbxbuild {

class TypeResolvedFile;

namespace Tool {

class SearchPaths;
class ToolContext;
class PrecompiledHeaderInfo;

class ClangResolver {
private:
    pbxspec::PBX::Compiler::shared_ptr _compiler;

public:
    ClangResolver(pbxspec::PBX::Compiler::shared_ptr const &compiler);
    ~ClangResolver();

public:
    void resolveSource(
        TypeResolvedFile const &input,
        std::vector<std::string> const &inputArguments,
        std::string const &outputBaseName,
        SearchPaths const &searchPaths,
        ToolContext *toolContext,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    ) const;
    void resolvePrecompiledHeader(
        PrecompiledHeaderInfo const &precompiledHeaderInfo,
        ToolContext *toolContext,
        pbxsetting::Environment const &environment,
        std::string const &workingDirectory
    ) const;

public:
    pbxspec::PBX::Compiler::shared_ptr const &compiler() const
    { return _compiler; }

public:
    static std::string ToolIdentifier()
    { return "com.apple.compilers.gcc"; }

public:
    static std::unique_ptr<ClangResolver>
    Create(Phase::PhaseEnvironment const &phaseEnvironment);
};

}
}

#endif // !__pbxbuild_ClangResolver_h
