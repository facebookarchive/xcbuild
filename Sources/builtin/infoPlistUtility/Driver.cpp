/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/infoPlistUtility/Driver.h>
#include <builtin/infoPlistUtility/Options.h>

using builtin::infoPlistUtility::Driver;
using builtin::infoPlistUtility::Options;

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
    return "builtin-infoPlistUtility";
}

int Driver::
run(std::vector<std::string> const &args)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    // TODO(grp): Implement info plist builtin.
    fprintf(stderr, "error: info plist not supported\n");
    return 1;
}
