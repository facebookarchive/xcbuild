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

class ToolEnvironment;
class OptionsResult;
class CommandLineResult;

class ToolResult {
public:
    static std::string
    LogMessage(ToolEnvironment const &toolEnvironment);

public:
    static ToolInvocation
    CreateInvocation(
        ToolEnvironment const &toolEnvironment,
        OptionsResult const &options,
        CommandLineResult const &commandLine,
        std::string const &logMessage,
        std::string const &workingDirectory,
        std::string const &dependencyInfo = "",
        std::vector<ToolInvocation::AuxiliaryFile> const &auxiliaryFiles = { }
    );
};

}
}

#endif // !__pbxbuild_ToolResult_h
