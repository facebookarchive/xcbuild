/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Escape.h>

using libutil::Escape;

std::string Escape::
Shell(std::string const &value)
{
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string digits = "0123456789";

    if (value.find_first_not_of(alphabet + digits + "@%_-+=:,./") == std::string::npos) {
        return value;
    } else {
        std::string result = value;
        std::string::size_type offset = 0;
        while ((offset = result.find("'", offset)) != std::string::npos) {
            result.replace(offset, 1, "'\"'\"'");
            offset += 5;
        }
        return "'" + result + "'";
    }
}

