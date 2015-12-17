/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/productPackagingUtility/Driver.h>
#include <builtin/productPackagingUtility/Options.h>

using builtin::productPackagingUtility::Driver;
using builtin::productPackagingUtility::Options;

Driver::
Driver()
{
}

Driver::
~Driver()
{
}

std::string Driver::
name()
{
    return "builtin-productPackagingUtility";
}

int Driver::
run(std::vector<std::string> const &args, std::unordered_map<std::string, std::string> const &environment)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    // TODO(grp): Implement product packaging builtin.
    fprintf(stderr, "error: product packaging not supported\n");
    return 1;
}
