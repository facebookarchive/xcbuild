/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/ResolverCommon.h>
#include <sstream>

namespace Tool = pbxbuild::Tool;

static std::vector<std::string> sCommonPathEnvironmentExtensions = {
    "$(PLATFORM_DEVELOPER_BIN_DIR)",
    "$(DEVELOPER_BIN_DIR)",
    "/usr/bin",
    "/bin",
    "/usr/sbin",
    "/sbin",
};

static std::vector<std::string> sScriptPathEnvironmentExtensions = {
    "$(DT_TOOLCHAIN_DIR)/usr/bin",
    "$(DT_TOOLCHAIN_DIR)/usr/libexec",
    "$(PLATFORM_DIR)/Developer/usr/bin",
    "$(PLATFORM_DIR)/usr/local/bin",
    "$(PLATFORM_DIR)/usr/bin",
    "$(PLATFORM_DIR)/usr/local/bin",
    "$(DEVELOPER_DIR)/usr/bin",
    "$(DEVELOPER_DIR)/usr/local/bin",
    "$(DEVELOPER_DIR)/Tools",
    "/usr/bin",
    "/bin",
    "/usr/sbin",
    "/sbin",
};

static std::string
_PathEnvironmentVariable(
    pbxsetting::Environment const &environment,
    std::vector<std::string> &extensions)
{
    std::string pathEnvironmentVariable;
    for (auto it = extensions.begin(); it != extensions.end(); it++) {
        if (pathEnvironmentVariable.length()) {
            pathEnvironmentVariable += ":";
        }
        pathEnvironmentVariable += environment.expand(pbxsetting::Value::Parse(*it));
    }
    return pathEnvironmentVariable;
}

std::unordered_map<std::string, std::string> Tool::ResolverCommon::
commonExtendedEnvironmentVariables(
    pbxsetting::Environment const &environment,
    const std::unordered_map<std::string, std::string> &environmentVariables)
{
    auto pathEnvironmentVariable = _PathEnvironmentVariable(environment, sCommonPathEnvironmentExtensions);
    auto pathExtendedEnvironmentVariables = environmentVariables;
    pathExtendedEnvironmentVariables["PATH"] += pathEnvironmentVariable;
    return pathExtendedEnvironmentVariables;
}

std::unordered_map<std::string, std::string> Tool::ResolverCommon::
scriptExtendedEnvironmentVariables(
    pbxsetting::Environment const &environment,
    const std::unordered_map<std::string, std::string> &environmentVariables)
{
    auto pathEnvironmentVariable = _PathEnvironmentVariable(environment, sScriptPathEnvironmentExtensions);
    auto pathExtendedEnvironmentVariables = environmentVariables;
    pathExtendedEnvironmentVariables["PATH"] += pathEnvironmentVariable;
    return pathExtendedEnvironmentVariables;
}
