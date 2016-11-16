/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/DefaultContext.h>
#include <libutil/FSUtil.h>

#include <mutex>
#include <sstream>
#include <unordered_set>
#include <cstring>
#include <cassert>

#if _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <linux/limits.h>
#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 16
#include <sys/auxv.h>
#endif
#endif

extern "C" char **environ;

using process::DefaultContext;
using libutil::FSUtil;

DefaultContext::
DefaultContext() :
    Context()
{
}

DefaultContext::
~DefaultContext()
{
}

std::string const &DefaultContext::
currentDirectory() const
{
    static std::string const *directory = nullptr;

    std::once_flag flag;
    std::call_once(flag, []{
        char path[PATH_MAX + 1];
        if (::getcwd(path, sizeof(path)) == nullptr) {
            path[0] = '\0';
        }

        directory = new std::string(path);
    });

    return *directory;
}

#if defined(__linux__)
static char initialWorkingDirectory[PATH_MAX] = { 0 };
__attribute__((constructor))
static void InitializeInitialWorkingDirectory()
{
    if (getcwd(initialWorkingDirectory, sizeof(initialWorkingDirectory)) == NULL) {
        abort();
    }
}

#if !(__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 16)
static char initialExecutablePath[PATH_MAX] = { 0 };
__attribute__((constructor))
static void InitialExecutablePathInitialize(int argc, char **argv)
{
    strncpy(initialExecutablePath, argv[0], sizeof(initialExecutablePath));
}
#endif
#endif

std::string const &DefaultContext::
executablePath() const
{
    static std::string const *executablePath = nullptr;

    std::once_flag flag;
    std::call_once(flag, []{
#if defined(__APPLE__)
        uint32_t size = 0;
        if (_NSGetExecutablePath(NULL, &size) != -1) {
            abort();
        }

        std::string absolutePath;
        absolutePath.resize(size);
        if (_NSGetExecutablePath(&absolutePath[0], &size) != 0) {
            abort();
        }
#elif defined(__linux__)
#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 16
        char const *path = reinterpret_cast<char const *>(getauxval(AT_EXECFN));
        if (path == NULL) {
            abort();
        }
#elif defined(__GLIBC__)
        char const *path = reinterpret_cast<char const *>(initialExecutablePath);
#else
#error Requires glibc on Linux.
#endif
        std::string absolutePath = FSUtil::ResolveRelativePath(std::string(path), std::string(initialWorkingDirectory));
#else
#error Unsupported platform.
#endif

        executablePath = new std::string(FSUtil::NormalizePath(absolutePath));
    });

    return *executablePath;
}

static int commandLineArgumentCount = 0;
static char **commandLineArgumentValues = NULL;

#if !defined(__linux__)
__attribute__((constructor))
#endif
static void CommandLineArgumentsInitialize(int argc, char **argv)
{
    commandLineArgumentCount = argc;
    commandLineArgumentValues = argv;
}

#if defined(__linux__)
__attribute__((section(".init_array"))) auto commandLineArgumentInitializer = &CommandLineArgumentsInitialize;
#endif

std::vector<std::string> const &DefaultContext::
commandLineArguments() const
{
    static std::vector<std::string> const *arguments = nullptr;

    std::once_flag flag;
    std::call_once(flag, []{
        arguments = new std::vector<std::string>(commandLineArgumentValues + 1, commandLineArgumentValues + commandLineArgumentCount);
    });

    return *arguments;
}

ext::optional<std::string> DefaultContext::
environmentVariable(std::string const &variable) const
{
    if (char *value = getenv(variable.c_str())) {
        return std::string(value);
    } else {
        return ext::nullopt;
    }
}

std::unordered_map<std::string, std::string> const &DefaultContext::
environmentVariables() const
{
    static std::unordered_map<std::string, std::string> const *environment = nullptr;

    std::once_flag flag;
    std::call_once(flag, []{
        std::unordered_map<std::string, std::string> values;

        for (char **current = environ; *current; current++) {
            std::string variable = *current;
            std::string::size_type offset = variable.find('=');

            std::string name = variable.substr(0, offset);
            std::string value = variable.substr(offset + 1);
            values.insert({ name, value });
        }

        environment = new std::unordered_map<std::string, std::string>(values);
    });

    return *environment;
}

