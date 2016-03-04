/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/copyStrings/Driver.h>

int
main(int argc, char **argv, char **envp)
{
    builtin::copyStrings::Driver driver;
    return driver.runc(argc, argv, envp);
}
