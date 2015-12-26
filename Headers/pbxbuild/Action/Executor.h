/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Action_Executor_h
#define __pbxbuild_Action_Executor_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Action/Formatter.h>
#include <pbxbuild/Build/Environment.h>
#include <pbxbuild/Build/Context.h>
#include <pbxbuild/BuildGraph.h>
#include <pbxbuild/Tool/Invocation.h>

namespace pbxbuild {

namespace Target { class Environment; }

namespace Action {

class Executor {
protected:
    std::shared_ptr<Formatter> _formatter;
    bool                       _dryRun;

protected:
    Executor(std::shared_ptr<Formatter> const &formatter, bool dryRun);

public:
    virtual ~Executor();

public:
    virtual bool build(
        Build::Environment const &buildEnvironment,
        Build::Context const &buildContext,
        BuildGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph) = 0;
};

}
}

#endif // !__pbxbuild_Action_Executor_h
