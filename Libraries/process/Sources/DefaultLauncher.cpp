/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/DefaultLauncher.h>
#include <libutil/Filesystem.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

// In most cases, size of pipe will be greater than one page,
#define PIPE_BUFFER_SIZE 4096

using process::DefaultLauncher;
using libutil::Filesystem;

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

    /* Setup parent-child stdout/stderr pipe. */
    int pfd[2];
    bool pipe_setup_success = true;
    if (pipe(pfd) == -1) {
        ::perror("pipe");
        pipe_setup_success = false;
    }

    /*
     * Fork new process.
     */
    pid_t pid = fork();
    if (pid < 0) {
        /* Fork failed. */
        return ext::nullopt;
    } else if (pid == 0) {
        /* Fork succeeded, new process. */
        if (pipe_setup_success) {
            /* Setup pipe to parent, redirecting both stdout and stderr */
            dup2(pfd[1], STDOUT_FILENO);
            dup2(pfd[1], STDERR_FILENO);
            close(pfd[0]);
            close(pfd[1]);
        } else {
            /* No parent-child pipe setup, just ignore outputs from child */
            int nullfd = open("/dev/null", O_WRONLY);
            if (nullfd == -1) {
                ::perror("open");
                ::_exit(1);
            }
            dup2(nullfd, STDOUT_FILENO);
            dup2(nullfd, STDERR_FILENO);
            close(nullfd);
        }

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
        if (pipe_setup_success) {
            close(pfd[1]);
            /* Read child's stdout/stderr through pipe, and output stdout */
            while (true) {
                char pin[PIPE_BUFFER_SIZE];
                int readlen = read(pfd[0], &pin, sizeof(pin));
                if (readlen > 0) {
                    fwrite(pin, readlen, 1, stdout);
                } else {
                    if (readlen != 0) {
                        ::perror("read");
                    }
                    break;
                }
            }
            close(pfd[0]);
        }

        int status;
        ::waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}
