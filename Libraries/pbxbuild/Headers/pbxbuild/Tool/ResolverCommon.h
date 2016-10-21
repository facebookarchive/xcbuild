/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_ResolverCommon_h
#define __pbxbuild_Tool_ResolverCommon_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Tool/Environment.h>

namespace pbxbuild {
namespace Tool {

class ResolverCommon {
public:
    static std::unordered_map<std::string, std::string> commonExtendedEnvironmentVariables(
        pbxsetting::Environment const &environment,
        const std::unordered_map<std::string, std::string> &environmentVariables);
    static std::unordered_map<std::string, std::string> scriptExtendedEnvironmentVariables(
        pbxsetting::Environment const &environment,
        const std::unordered_map<std::string, std::string> &environmentVariables);
};

}
}

#endif
