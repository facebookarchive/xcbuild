/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_Subprocess_h
#define __libutil_Subprocess_h

#include <string>
#include <vector>
#include <unordered_map>

namespace libutil {

class Filesystem;

class Subprocess {
private:
    int _exitcode;

public:
    Subprocess();

public:
    inline int exitcode() const
    { return _exitcode; }

public:
    bool execute(Filesystem *filesystem,
                 std::string const &path,
                 std::vector<std::string> const &arguments,
                 std::unordered_map<std::string, std::string> const &environment,
                 std::string const &directory,
                 std::istream *input = nullptr,
                 std::ostream *output = nullptr,
                 std::ostream *error = nullptr);
};

}

#endif  // !__libutil_Subprocess_h
