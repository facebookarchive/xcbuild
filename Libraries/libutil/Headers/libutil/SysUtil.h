/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_SysUtil_h
#define __libutil_SysUtil_h

#include <string>
#include <vector>
#include <unordered_map>

namespace libutil {

class SysUtil {
public:
    /*
     * The path to the running executable.
     */
    static std::string GetExecutablePath();

    /*
     * The current directory of the process.
     */
    static std::string GetCurrentDirectory();

public:
    /*
     * The default environment search paths.
     */
    static std::vector<std::string> GetExecutablePaths();

    /*
     * All environment variables.
     */
    static std::unordered_map<std::string, std::string> GetEnvironmentVariables();

public:
    /*
     * Active user name.
     */
    static std::string GetUserName();

    /*
     * Active group name.
     */
    static std::string GetGroupName();

    /*
     * Active user ID.
     */
    static int32_t GetUserID();

    /*
     * Active group ID.
     */
    static int32_t GetGroupID();
};

}

#endif  // !__libutil_SysUtil_h
