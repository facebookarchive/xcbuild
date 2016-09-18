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
#include <ext/optional>

namespace libutil {

class SysUtil {
public:
    /*
     * The path to the running executable.
     */
    std::string executablePath() const;

    /*
     * The current directory of the process.
     */
    std::string currentDirectory() const;

public:
    /*
     * All environment variables.
     */
    std::unordered_map<std::string, std::string> environmentVariables() const;

public:
    /*
     * Active user ID.
     */
    int32_t userID() const;

    /*
     * Active group ID.
     */
    int32_t groupID() const;

    /*
     * Active user name.
     */
    std::string userName() const;

    /*
     * Active group name.
     */
    std::string groupName() const;

public:
    /*
     * Single environment variable.
     */
    ext::optional<std::string> environmentVariable(std::string const &variable) const;

    /*
     * The default environment search paths.
     */
    std::vector<std::string> executableSearchPaths() const;

    /*
     * The home directory from the environment.
     */
    std::string userHomeDirectory() const;

public:
    /*
     * Get the system instance.
     */
    static SysUtil const *GetDefault();
};

}

#endif  // !__libutil_SysUtil_h
