/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/Driver.h>
#include <libutil/DefaultFilesystem.h>
#include <libutil/DefaultProcessContext.h>

using libutil::DefaultFilesystem;
using libutil::DefaultProcessContext;

int
main(int argc, char **argv)
{
    DefaultFilesystem filesystem = DefaultFilesystem();
    DefaultProcessContext processContext = DefaultProcessContext();
    return xcdriver::Driver::Run(&processContext, &filesystem);
}
