/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Action/Executor.h>

using pbxbuild::Action::Executor;

Executor::
Executor(std::shared_ptr<Formatter> const &formatter, bool dryRun) :
    _formatter(formatter),
    _dryRun   (dryRun)
{
}

Executor::
~Executor()
{
}
