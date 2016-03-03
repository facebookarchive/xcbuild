/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Subprocess.h>
#include <libutil/FSUtil.h>

#include <sys/wait.h>
#include <unistd.h>

using libutil::Subprocess;

Subprocess::Subprocess() :
    _exitcode(0)
{
}

bool Subprocess::
execute(
    std::string const &path,
    std::vector<std::string> const &arguments,
    std::unordered_map<std::string, std::string> const &environment,
    std::string const &directory,
    std::istream *input,
    std::ostream *output,
    std::ostream *error)
{
    if (!FSUtil::TestForExecute(path)) {
        return false;
    }

    std::vector <char const *> exec_args;
    exec_args.push_back(path.c_str());
    for (std::string const &I : arguments) {
        exec_args.push_back(I.c_str());
    }
    exec_args.push_back(nullptr);

    std::vector<std::string> env_values;
    for (auto const &I : environment) {
        env_values.push_back(I.first + "=" + I.second);
    }

    std::vector <char const *> exec_env;
    for (auto const &I : env_values) {
        exec_env.push_back(I.c_str());
    }
    exec_env.push_back(nullptr);

    char const *work_dir = (!directory.empty() ? directory.c_str() : nullptr);

    pid_t pid;
    int  ofds[2];
    int  efds[2];

    if (::pipe(ofds) != 0) {
        return false;
    }

    if (::pipe(efds) != 0) {
        ::close(ofds[1]);
        ::close(ofds[0]);
        return false;
    }

    pid = fork();
    if (pid < 0) {
        ::close(ofds[1]);
        ::close(ofds[0]);
        ::close(efds[1]);
        ::close(efds[0]);
        return false;
    }

    if (pid == 0) {
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

        if (work_dir != nullptr) {
            ::chdir(work_dir);
        }

        ::execve(path.c_str(), (char *const *)exec_args.data(), (char *const *)exec_env.data());
        ::_exit(-1);
        return false;
    }

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
            if (nread <= 0)
                break;

            buf[nread] = '\0';
            *output << buf;
        }
    }

    if (error != nullptr) {
        for (;;) {
            char    buf[16384];
            ssize_t nread;

            nread = ::read(efds[0], buf, sizeof(buf));
            if (nread <= 0)
                break;

            buf[nread] = '\0';
            *error << buf;
        }
    }

    ::close(ofds[0]);
    ::close(efds[0]);

    int status;
    ::waitpid(pid, &status, 0);
    _exitcode = WEXITSTATUS(status);

    return true;
}
