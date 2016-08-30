/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/Driver.h>
#include <libutil/DefaultFilesystem.h>

using libutil::DefaultFilesystem;

int
main(int argc, char **argv)
{
    DefaultFilesystem filesystem = DefaultFilesystem();
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);
    return xcdriver::Driver::Run(&filesystem, args);
}
