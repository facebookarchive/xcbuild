/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_Context_h
#define __pbxbuild_Tool_Context_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Tool/Invocation.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/CompilationInfo.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>

namespace pbxbuild {
namespace Tool {

class Context {
private:
    std::string                 _workingDirectory;

private:
    HeadermapInfo               _headermapInfo;
    CompilationInfo             _compilationInfo;
    SearchPaths                 _searchPaths;
    std::vector<std::string>    _additionalInfoPlistContents;

private:
    std::vector<Tool::Invocation const>                                                _invocations;
    std::map<std::pair<std::string, std::string>, std::vector<Tool::Invocation const>> _variantArchitectureInvocations;

public:
    Context(std::string const &workingDirectory);
    ~Context();

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
    std::vector<std::string> const &additionalInfoPlistContents() const
    { return _additionalInfoPlistContents; }

public:
    HeadermapInfo &headermapInfo()
    { return _headermapInfo; }
    CompilationInfo &compilationInfo()
    { return _compilationInfo; }
    SearchPaths &searchPaths()
    { return _searchPaths; }
    std::vector<std::string> &additionalInfoPlistContents()
    { return _additionalInfoPlistContents; }

public:
    std::vector<Tool::Invocation const> const &invocations() const
    { return _invocations; }
    std::map<std::pair<std::string, std::string>, std::vector<Tool::Invocation const>> const &variantArchitectureInvocations() const
    { return _variantArchitectureInvocations; }

public:
    std::vector<Tool::Invocation const> &invocations()
    { return _invocations; }
    std::map<std::pair<std::string, std::string>, std::vector<Tool::Invocation const>> &variantArchitectureInvocations()
    { return _variantArchitectureInvocations; }
};

}
}

#endif // !__pbxbuild_Tool_Context_h
