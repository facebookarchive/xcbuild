/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_ToolResult_h
#define __pbxbuild_ToolResult_h

#include <pbxbuild/Base.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {
namespace Tool {

class Environment;
class OptionsResult;
class CommandLineResult;

class ToolResult {
public:
    static std::string
    LogMessage(Tool::Environment const &toolEnvironment);
};

}
}

#endif // !__pbxbuild_ToolResult_h
