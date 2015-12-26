/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/ToolResult.h>
#include <pbxbuild/Tool/OptionsResult.h>
#include <pbxbuild/Tool/Environment.h>
#include <pbxbuild/Tool/CommandLineResult.h>
#include <sstream>

namespace Tool = pbxbuild::Tool;

std::string Tool::ToolResult::
LogMessage(Tool::Environment const &toolEnvironment)
{
    pbxsetting::Environment const &environment = toolEnvironment.toolEnvironment();
    std::string ruleName = environment.expand(toolEnvironment.tool()->ruleName());
    std::string ruleFormat = environment.expand(toolEnvironment.tool()->ruleFormat());

    if (!ruleName.empty()) {
        return ruleName;
    } else if (!ruleFormat.empty()) {
        return ruleFormat;
    } else {
        return std::string();
    }
}

