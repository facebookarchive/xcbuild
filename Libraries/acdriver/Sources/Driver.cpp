/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <acdriver/Driver.h>
#include <acdriver/Options.h>

using acdriver::Driver;
using acdriver::Options;

Driver::
Driver()
{
}

Driver::
~Driver()
{
}

int Driver::
Run(std::vector<std::string> const &args)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    fprintf(stderr, "warning: actool not yet implemented\n");
    return 0;
}
