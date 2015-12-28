/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxsetting_Type_h
#define __pbxsetting_Type_h

#include <pbxsetting/Base.h>

#include <string>
#include <vector>

namespace pbxsetting {

/*
 * Parses various types of data out of setting values. By default, all build
 * settings are treated as strings, as returned from `Environment::resolve()`.
 * However, in some contexts settings should be treated as integers, booleans,
 * or lists of strings. This is a standardized way to parse them into those.
 */
class Type {
private:
    Type();
    ~Type();

public:
    /*
     * Parses out a boolean. Any capitalization of "YES" or "true" will become
     * true, otherwise this will return false.
     */
    static bool
    ParseBoolean(std::string const &value);

    /*
     * Parses out an integer. Returns 0 if an integer could not be parsed.
     */
    static int64_t
    ParseInteger(std::string const &value);

    /*
     * Parses a space-separated list of strings (or path names). Simple quoting
     * can be used to escape spaces in strings, and backslashes to escape quotes.
     */
    static std::vector<std::string>
    ParseList(std::string const &value);
};

}

#endif  // !__pbxsetting_Type_h
