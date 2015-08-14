// Copyright 2013-present Facebook. All Rights Reserved.

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
execute(std::string const &path, string_vector const &arguments, 
        std::istream *input, std::ostream *output)
{
    if (!FSUtil::TestForExecute(path))
        return false;

    std::vector <char const *> exec_args;
    exec_args.push_back(path.c_str());
    for (auto I : arguments) {
        exec_args.push_back(I.c_str());
    }
    exec_args.push_back(nullptr);

    pid_t pid;
    int   fds[2];

    if (::pipe(fds) != 0)
        return false;

    pid = fork();
    if (pid < 0) {
        ::close(fds[1]);
        ::close(fds[0]);
        return false;
    }

    if (pid == 0) {
        if (input == nullptr) {
            ::close(fds[0]);
        }

        ::close(0);
        ::close(1);
        ::close(2);

        if (input != nullptr) {
            ::dup2(fds[0], 0);
        }
        if (output != nullptr) {
            ::dup2(fds[1], 1);
        }

        ::execv(path.c_str(), (char * const *)&exec_args[0]);
        ::_exit(-1);
        return false;
    }

    if (input != nullptr) {
        // TODO!
    }

    ::close(fds[1]);

    if (output != nullptr) {
        for (;;) {
            char    buf[16384];
            ssize_t nread;

            nread = ::read(fds[0], buf, sizeof(buf));
            if (nread <= 0)
                break;

            buf[nread] = '\0';
            *output << buf;
        }
    }

    ::close(fds[0]);

    int status;
    ::waitpid(pid, &status, 0);
    _exitcode = WEXITSTATUS(status);

    return true;
}
