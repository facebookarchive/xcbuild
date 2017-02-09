/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __process_LaunchResult_h
#define __process_LaunchResult_h

#include <process/Context.h>

#include <sstream>
#include <ext/optional>

namespace libutil { class Filesystem; }

namespace process {

/*
 * The result of a successfully launched process: its exit code and output.
 */
class LaunchResult {
private:
    int _exitCode;
    std::string _standardOutput;
    std::string _standardError;

public:
    LaunchResult(
        int exitCode,
        std::string const &standardOutput,
        std::string const &standardError);
    ~LaunchResult();

    int exitCode() const
    { return _exitCode; }

    std::string const &standardOutput() const
    { return _standardOutput; }

    std::string const &standardError() const
    { return _standardError; }
};

}

#endif  // !__process_LaunchResult_h
