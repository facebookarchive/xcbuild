/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <xcexecution/Executor.h>

using xcexecution::Executor;

Executor::
Executor(std::shared_ptr<xcformatter::Formatter> const &formatter, bool dryRun, bool generate) :
    _formatter(formatter),
    _dryRun   (dryRun),
    _generate (generate)
{
}

Executor::
~Executor()
{
}
