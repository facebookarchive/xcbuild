/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_ToolContext_h
#define __pbxbuild_ToolContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/CompilationInfo.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>

namespace pbxbuild {
namespace Tool {

class ToolContext {
private:
    std::string                 _workingDirectory;

private:
    HeadermapInfo               _headermapInfo;
    CompilationInfo             _compilationInfo;
    SearchPaths                 _searchPaths;

private:
    std::vector<ToolInvocation> _invocations;
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> _variantArchitectureInvocations;

public:
    ToolContext(std::string const &workingDirectory);
    ~ToolContext();

public:
    std::string const &workingDirectory() const
    { return _workingDirectory; }

public:
    HeadermapInfo const &headermapInfo() const
    { return _headermapInfo; }
    CompilationInfo const &compilationInfo() const
    { return _compilationInfo; }
    SearchPaths const &searchPaths() const
    { return _searchPaths; }

public:
    HeadermapInfo &headermapInfo()
    { return _headermapInfo; }
    CompilationInfo &compilationInfo()
    { return _compilationInfo; }
    SearchPaths &searchPaths()
    { return _searchPaths; }

public:
    std::vector<ToolInvocation> const &invocations() const
    { return _invocations; }
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> const &variantArchitectureInvocations() const
    { return _variantArchitectureInvocations; }

public:
    std::vector<ToolInvocation> &invocations()
    { return _invocations; }
    std::map<std::pair<std::string, std::string>, std::vector<ToolInvocation>> &variantArchitectureInvocations()
    { return _variantArchitectureInvocations; }
};

}
}

#endif // !__pbxbuild_ToolContext_h
