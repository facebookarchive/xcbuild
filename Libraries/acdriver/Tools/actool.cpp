/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <acdriver/Driver.h>

int
main(int argc, char **argv)
{
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);
    return acdriver::Driver::Run(args);
}
