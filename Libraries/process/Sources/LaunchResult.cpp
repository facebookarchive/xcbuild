/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/LaunchResult.h>

using process::LaunchResult;

LaunchResult::
LaunchResult(
    int exitCode,
    std::string const &standardOutput,
    std::string const &standardError) :
    _exitCode      (exitCode),
    _standardOutput(standardOutput),
    _standardError (standardError)
{
}

LaunchResult::
~LaunchResult()
{
}


