/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/DefaultLauncher.h>
#include <process/Context.h>
#include <libutil/Filesystem.h>

#if _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

using process::DefaultLauncher;
using process::Context;
using libutil::Filesystem;

#if _WIN32
using WideString = std::basic_string<std::remove_const<std::remove_pointer<LPCWSTR>::type>::type>;

static WideString
StringToWideString(std::string const &str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
    WideString wide = WideString();
    wide.resize(size);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wide[0], size);
    return wide;
}
#endif

DefaultLauncher::
DefaultLauncher() :
    Launcher()
{
}

DefaultLauncher::
~DefaultLauncher()
{
}

ext::optional<int> DefaultLauncher::
launch(Filesystem *filesystem, Context const *context)
{
#if _WIN32
    WideString arguments;
    for (std::string const &argument : context->commandLineArguments()) {
        if (&argument != &context->commandLineArguments().front()) {
            arguments += static_cast<wchar_t>(' ');
        }

        if (!argument.empty() && argument.find_first_of(" \t\n\v\"") == std::string::npos) {
            arguments += StringToWideString(argument);
        } else {
            arguments += static_cast<wchar_t>('"');

            WideString wide = StringToWideString(argument);
            WideString special = StringToWideString("\"\\");
            for (auto it = wide.begin(); it != wide.end(); ++it) {
                /* Find the first backslash or quote. */
                size_t first_ = wide.find_first_of(special, it - wide.begin());
                auto first = (first_ == std::string::npos ? wide.end() : wide.begin() + first_);

                /* Copy up to the first special character. */
                arguments.insert(arguments.end(), it, first);

                if (first == wide.end()) {
                    /* Nothing special; jump to the end. */
                    it = std::prev(first);
                } else if (*first == static_cast<wchar_t>('"')) {
                    /* Escape and append the quote itself. */
                    arguments += static_cast<wchar_t>('\\');
                    arguments += *first;

                    /* Move past the quote. */
                    it = first;
                } else if (*first == static_cast<wchar_t>('\\')) {
                    /* Find next non-backslash character after the first backslash. */
                    size_t after_ = wide.find_first_not_of(static_cast<wchar_t>('\\'), first - wide.begin());
                    auto after = (after_ == std::string::npos ? wide.end() : wide.begin() + after_);

                    if (after == wide.end() || *after == static_cast<wchar_t>('"')) {
                        /* Duplicate (escape) all backslashes before (possibly ending) quote. */
                        arguments.insert(arguments.end(), first, after);
                        arguments.insert(arguments.end(), first, after);
                    } else {
                        /* Not before quote, no need to escape backslashes. */
                        arguments.insert(arguments.end(), first, after);
                    }

                    /* Jump to the next non-backslash character. */
                    it = std::prev(after);
                } else {
                    abort();
                }
            }

            arguments += static_cast<wchar_t>('"');
        }
    }

    WideString environment;
    for (auto const &entry : context->environmentVariables()) {
        environment += StringToWideString(entry.first);
        environment += StringToWideString("=");
        environment += StringToWideString(entry.second);
        environment += static_cast<wchar_t>('\0');
    }

    WideString executablePath = StringToWideString(context->executablePath());
    WideString currentDirectory = StringToWideString(context->currentDirectory());

    STARTUPINFOW startup;
    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(startup);

    PROCESS_INFORMATION process;
    if (!CreateProcessW(
        executablePath.c_str(),
        &arguments[0],
        nullptr,
        nullptr,
        FALSE,
        CREATE_UNICODE_ENVIRONMENT,
        static_cast<LPVOID>(&environment[0]),
        currentDirectory.c_str(),
        &startup,
        &process)) {
        return ext::nullopt;
    }

    if (!CloseHandle(process.hThread)) {
        /* Error, but process started. Ignore. */
    }

    if (!CloseHandle(process.hProcess)) {
        /* Error, but process started. Ignore. */
    }

    return static_cast<int>(process.dwProcessId);
#else
    /*
     * Extract input data for exec, so no C++ is required after fork.
     */
    std::string path = context->executablePath();
    if (!filesystem->isExecutable(path)) {
        return ext::nullopt;
    }
    char const *cPath = path.c_str();

    std::string directory = context->currentDirectory();
    char const *cDirectory = directory.c_str();

    /* Compute command-line arguments. */
    std::vector<char const *> execArgs;
    execArgs.push_back(cPath);

    std::vector<std::string> arguments = context->commandLineArguments();
    for (std::string const &argument : arguments) {
        execArgs.push_back(argument.c_str());
    }

    execArgs.push_back(nullptr);
    char *const *cExecArgs = const_cast<char *const *>(execArgs.data());

    /* Compute environment variables. */
    std::vector<std::string> envValues;
    for (auto const &value : context->environmentVariables()) {
        envValues.push_back(value.first + "=" + value.second);
    }

    std::vector<char const *> execEnv;
    for (auto const &value : envValues) {
        execEnv.push_back(value.c_str());
    }
    execEnv.push_back(nullptr);
    char *const *cExecEnv = const_cast<char *const *>(execEnv.data());

    /* Compute user. */
    uid_t uid = context->userID();
    gid_t gid = context->groupID();

    /*
     * Fork new process.
     */
    pid_t pid = fork();
    if (pid < 0) {
        /* Fork failed. */
        return ext::nullopt;
    } else if (pid == 0) {
        /* Fork succeeded, new process. */
        if (::chdir(cDirectory) == -1) {
            ::perror("chdir");
            ::_exit(1);
        }

        if (::setuid(uid) == -1) {
            ::perror("setuid");
            ::_exit(1);
        }

        if (::setgid(gid) == -1) {
            ::perror("setgid");
            ::_exit(1);
        }

        ::execve(cPath, cExecArgs, cExecEnv);
        ::_exit(-1);

        return ext::nullopt;
    } else {
        /* Fork succeeded, existing process. */
        int status;
        ::waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
#endif
}
