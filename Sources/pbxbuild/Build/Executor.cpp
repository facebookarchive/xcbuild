// Copyright 2013-present Facebook. All Rights Reserved.

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
