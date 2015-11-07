/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Options.h>

using libutil::Options;

std::pair<bool, std::string> Options::
NextString(std::string *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it, bool allowDuplicate)
{
    std::string const &arg = **it;
    ++(*it);

    if ((*it) != args.end()) {
        if (allowDuplicate || result->empty()) {
            *result = **it;
            return std::make_pair(true, std::string());
        } else {
            return std::make_pair(false, "duplicate argument " + arg);
        }
    } else {
        return std::make_pair(false, "missing argument value for argument " + arg);
    }
}

std::pair<bool, std::string> Options::
NextInt(int *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it, bool allowDuplicate)
{
    std::string str = std::to_string(*result);
    std::pair<bool, std::string> success = NextString(&str, args, it, allowDuplicate);
    if (success.first) {
        *result = std::atoi(str.c_str());
    }
    return success;
}

std::pair<bool, std::string> Options::
NextBool(bool *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it, bool allowDuplicate)
{
    std::string str = *result ? "YES" : "NO";
    std::pair<bool, std::string> success = NextString(&str, args, it, allowDuplicate);
    if (success.first) {
        if (str == "YES") {
            *result = true;
        } else if (str == "NO") {
            *result = false;
        } else {
            success = std::make_pair(false, "invalid value " + str + " for boolean argument");
        }
    }
    return success;
}

std::pair<bool, std::string> Options::
MarkBool(bool *result, std::string const &arg, bool allowDuplicate)
{
    if (allowDuplicate || !*result) {
        *result = true;
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "duplicate argument " + arg);
    }
}

