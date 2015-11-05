/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Build/Executor.h>

using pbxbuild::Build::Executor;

Executor::
Executor(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun) :
    _buildEnvironment(buildEnvironment),
    _buildContext    (buildContext),
    _formatter       (formatter),
    _dryRun          (dryRun)
{
}

Executor::
~Executor()
{
}
