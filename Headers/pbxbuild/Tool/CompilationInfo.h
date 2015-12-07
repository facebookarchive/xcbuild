/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_CompilationInfo_h
#define __pbxbuild_CompilationInfo_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class PrecompiledHeaderInfo;

class CompilationInfo {
private:
    std::shared_ptr<PrecompiledHeaderInfo> _precompiledHeaderInfo;

private:
    std::string                            _linkerDriver;
    std::unordered_set<std::string>        _linkerArguments;

public:
    CompilationInfo();
    ~CompilationInfo();

public:
    std::shared_ptr<PrecompiledHeaderInfo> const &precompiledHeaderInfo() const
    { return _precompiledHeaderInfo; }

public:
    std::string const &linkerDriver() const
    { return _linkerDriver; }
    std::unordered_set<std::string> const &linkerArguments() const
    { return _linkerArguments; }

public:
    std::shared_ptr<PrecompiledHeaderInfo> &precompiledHeaderInfo()
    { return _precompiledHeaderInfo; }

public:
    std::string &linkerDriver()
    { return _linkerDriver; }
    std::unordered_set<std::string> &linkerArguments()
    { return _linkerArguments; }
};

}
}

#endif // !__pbxbuild_CompilationInfo_h
