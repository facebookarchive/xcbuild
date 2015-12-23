/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Executor_h
#define __pbxbuild_Executor_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Build/Formatter.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/BuildContext.h>
#include <pbxbuild/ToolInvocation.h>

namespace pbxbuild {

class TargetEnvironment;

namespace Build {

class Executor {
protected:
    BuildEnvironment           _buildEnvironment;
    BuildContext               _buildContext;

protected:
    std::shared_ptr<Formatter> _formatter;
    bool                       _dryRun;

protected:
    Executor(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun);
    ~Executor();

public:
    virtual void prepare() = 0;
    virtual void finish() = 0;

public:
    virtual bool buildTarget(
        pbxproj::PBX::Target::shared_ptr const &target,
        TargetEnvironment const &targetEnvironment,
        std::vector<ToolInvocation const> const &invocations
    ) = 0;
};

}
}

#endif // !__pbxbuild_Executor_h
