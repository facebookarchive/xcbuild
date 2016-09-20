/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/DefaultLauncher.h>
#include <libutil/Filesystem.h>

#include <sys/wait.h>
#include <unistd.h>

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
launch(
    Filesystem *filesystem,
    Context const *context,
    std::istream *input,
    std::ostream *output,
    std::ostream *error)
{
    /*
     * Extract input data for exec, so no C++ is required after fork.
     */
    std::string path = context->executablePath();
    if (!filesystem->isExecutable(path)) {
        return ext::nullopt;
    }
    char const *path_str = path.c_str();

    std::string directory = context->currentDirectory();
    char const *work_dir = directory.c_str();

    /* Compute command-line arguments. */
    std::vector<char const *> exec_args;
    exec_args.push_back(path_str);

    std::vector<std::string> arguments = context->commandLineArguments();
    for (std::string const &argument : arguments) {
        exec_args.push_back(argument.c_str());
    }

    exec_args.push_back(nullptr);
    char *const *exec_args_data = const_cast<char *const *>(exec_args.data());

    /* Compute environment variables. */
    std::vector<std::string> env_values;
    for (auto const &value : context->environmentVariables()) {
        env_values.push_back(value.first + "=" + value.second);
    }

    std::vector<char const *> exec_env;
    for (auto const &value : env_values) {
        exec_env.push_back(value.c_str());
    }
    exec_env.push_back(nullptr);
    char *const *exec_env_data = const_cast<char *const *>(exec_env.data());

    /* Compute user. */
    uid_t uid = context->userID();
    gid_t gid = context->groupID();

    /*
     * Setup output pipe.
     */
    int ofds[2];
    int efds[2];

    if (::pipe(ofds) != 0) {
        return ext::nullopt;
    }

    if (::pipe(efds) != 0) {
        ::close(ofds[1]);
        ::close(ofds[0]);
        return ext::nullopt;
    }

    /*
     * Fork new process.
     */
    pid_t pid = fork();
    if (pid < 0) {
        /*
         * Fork failed, clean up.
         */
        ::close(ofds[1]);
        ::close(ofds[0]);
        ::close(efds[1]);
        ::close(efds[0]);

        return ext::nullopt;
    } else if (pid == 0) {
        /*
         * Fork succeeded, new process. Execute binary with requested parameters.
         */
        ::close(ofds[0]);
        ::close(efds[0]);

        if (output != nullptr) {
            ::close(1);
            ::dup2(ofds[1], 1);
        }
        if (error != nullptr) {
            ::close(2);
            ::dup2(efds[1], 2);
        }

        if (input != nullptr) {
            // TODO!
        }

        if (::chdir(work_dir) == -1) {
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

        ::execve(path_str, exec_args_data, exec_env_data);
        ::_exit(-1);

        return ext::nullopt;
    } else {
        /*
         * Fork succeeded, existing process.
         */
        if (input != nullptr) {
            // TODO!
        }

        ::close(ofds[1]);
        ::close(efds[1]);

        if (output != nullptr) {
            for (;;) {
                char    buf[16384];
                ssize_t nread;

                nread = ::read(ofds[0], buf, sizeof(buf));
                if (nread <= 0) {
                    break;
                }

                buf[nread] = '\0';
                *output << buf;
            }
        }

        if (error != nullptr) {
            for (;;) {
                char    buf[16384];
                ssize_t nread;

                nread = ::read(efds[0], buf, sizeof(buf));
                if (nread <= 0) {
                    break;
                }

                buf[nread] = '\0';
                *error << buf;
            }
        }

        ::close(ofds[0]);
        ::close(efds[0]);

        int status;
        ::waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}
