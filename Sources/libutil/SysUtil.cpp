/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/SysUtil.h>
#include <libutil/FSUtil.h>

#include <sstream>

#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__GLIBC__)
#include <sys/auxv.h>
#endif

extern char **environ;

using libutil::SysUtil;
using libutil::FSUtil;

std::unordered_map<std::string, std::string> SysUtil::
EnvironmentVariables()
{
    std::unordered_map<std::string, std::string> environment;

    for (char **current = environ; *current; current++) {
        std::string variable = *current;
        std::string::size_type offset = variable.find('=');

        std::string name = variable.substr(0, offset);
        std::string value = variable.substr(offset + 1);
        environment.insert(std::make_pair(name, value));
    }

    return environment;
}

#if defined(__GLIBC__)
char initialWorkingDirectory[PATH_MAX] = { 0 };

__attribute__((constructor))
static void GetExecutablePathInitialize()
{
    strncpy(initialWorkingDirectory, getcwd(), sizeof(initialWorkingDirectory));
}
#endif

std::string SysUtil::
GetExecutablePath()
{
#if defined(__APPLE__)
    uint32_t size = 0;
    if (_NSGetExecutablePath(NULL, &size) != -1) {
        abort();
    }

    std::string buffer;
    buffer.resize(size);
    if (_NSGetExecutablePath(&buffer[0], &size) != 0) {
        abort();
    }

    return buffer;
#elif defined(__GLIBC__)
    char const *path = getauxval(AT_EXECFN);
    if (path == NULL) {
        assert(false);
        abort();
    }

    return FSUtil::ResolveRelativePath(std::string(path), std::string(initialWorkingDirectory));
#else
#error Unsupported platform.
#endif
}

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
