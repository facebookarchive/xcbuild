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
    inline bool run(std::string const &path,
                    string_vector const &arguments, 
                    std::istream &input,
                    std::ostream &output)
    {
        return execute(path, arguments, &input, &output);
    }

    inline bool run(std::string const &path,
                    std::istream &input,
                    std::ostream &output)
    {
        return execute(path, string_vector(), &input, &output);
    }

public:
    inline bool run(std::string const &path,
                    string_vector const &arguments,
                    std::ostream &output)
    {
        return execute(path, arguments, nullptr, &output);
    }

    inline bool run(std::string const &path,
                    std::ostream &output)
    {
        return execute(path, string_vector(), nullptr, &output);
    }

public:
    inline bool run(std::string const &path,
                    string_vector const &arguments,
                    std::istream &input)
    {
        return execute(path, arguments, &input, nullptr);
    }

    inline bool run(std::string const &path,
                    std::istream &input)
    {
        return execute(path, string_vector(), &input, nullptr);
    }

public:
    inline bool run(std::string const &path,
                    string_vector const &arguments)
    {
        return execute(path, arguments, nullptr, nullptr);
    }

    inline bool run(std::string const &path)
    {
        return execute(path, string_vector(), nullptr, nullptr);
    }

private:
    bool execute(std::string const &path,
                 string_vector const &arguments, 
                 std::istream *input,
                 std::ostream *output);
};

}

#endif  // !__libutil_Subprocess_h
