// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_Subprocess_h
#define __libutil_Subprocess_h

#include <libutil/Base.h>

namespace libutil {

class Subprocess {
private:
    int _exitcode;

public:
    Subprocess();

public:
    inline int exitcode() const
    { return _exitcode; }

public:
    bool execute(std::string const &path,
                 std::istream *input = nullptr,
                 std::ostream *output = nullptr,
                 std::ostream *error = nullptr)
    {
        return execute(path, { }, { }, "", input, output, error);
    }

    bool execute(std::string const &path,
                 string_vector const &arguments,
                 std::istream *input = nullptr,
                 std::ostream *output = nullptr,
                 std::ostream *error = nullptr)
    {
        return execute(path, arguments, { }, "", input, output, error);
    }

    bool execute(std::string const &path,
                 string_vector const &arguments,
                 std::unordered_map<std::string, std::string> const &environment,
                 std::istream *input = nullptr,
                 std::ostream *output = nullptr,
                 std::ostream *error = nullptr)
    {
        return execute(path, arguments, environment, "", input, output, error);
    }

public:
    bool execute(std::string const &path,
                 string_vector const &arguments,
                 std::unordered_map<std::string, std::string> const &environment,
                 std::string const &directory,
                 std::istream *input = nullptr,
                 std::ostream *output = nullptr,
                 std::ostream *error = nullptr);
};

}

#endif  // !__libutil_Subprocess_h
