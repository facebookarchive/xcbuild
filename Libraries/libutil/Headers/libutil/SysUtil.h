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
#include <unordered_map>

namespace libutil {

class SysUtil {
public:
    static std::unordered_map<std::string, std::string> EnvironmentVariables();

public:
    static std::string GetExecutablePath();

public:
    static std::string GetUserName();
    static std::string GetGroupName();

    static int32_t GetUserID();
    static int32_t GetGroupID();

public:
    static void Sleep(uint64_t us, bool interruptible = false);
};

}

#endif  // !__libutil_SysUtil_h
