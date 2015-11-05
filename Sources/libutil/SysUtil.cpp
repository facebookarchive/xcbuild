/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/SysUtil.h>

#include <sstream>

#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

using libutil::SysUtil;

std::string SysUtil::
GetUserName()
{
    std::string result;

    struct passwd const *pw = ::getpwuid(::getuid());
    if (pw != nullptr) {
        if (pw->pw_name != nullptr) {
            result = pw->pw_name;
        }
    }
    if (result.empty()) {
        std::ostringstream os;
        os << ::getuid();
        result = os.str();
    }

    ::endpwent();

    return result;
}

std::string SysUtil::
GetGroupName()
{
    std::string result;

    struct group const *gr = ::getgrgid(::getgid());
    if (gr != nullptr) {
        if (gr->gr_name != nullptr) {
            result = gr->gr_name;
        }
    }
    if (result.empty()) {
        std::ostringstream os;
        os << ::getgid();
        result = os.str();
    }

    ::endgrent();

    return result;
}

int32_t SysUtil::
GetUserID()
{
    return ::getuid();
}

int32_t SysUtil::
GetGroupID()
{
    return ::getgid();
}

void SysUtil::
Sleep(uint64_t us, bool interruptible)
{
    struct timeval tv;

    tv.tv_sec  = us / 1000000;
    tv.tv_usec = us % 1000000;

    for (;;) {
        int rc = ::select(0, nullptr, nullptr, nullptr, &tv);
        if (rc < 0) {
            if (errno == EINTR && !interruptible)
                continue;

            break;
        }
    }
}
