/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#ifndef __libutil_Escape_h
#define __libutil_Escape_h

#include <string>

namespace libutil {

class Escape {
private:
    Escape();
    ~Escape();

public:
    /*
     * Shell-escapes a string.
     */
    static std::string
    Shell(std::string const &value);

    /*
     * Escape a file path for a Makefile.
     */
    static std::string
    Makefile(std::string const &value);
};

}

#endif  // !__libutil_Escape_h
