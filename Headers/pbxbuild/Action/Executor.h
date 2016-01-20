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
#include <pbxbuild/DirectedGraph.h>
#include <pbxbuild/Tool/Invocation.h>

namespace pbxbuild {

namespace Target { class Environment; }

namespace Action {

/*
 * Abstract executor for builds. The executor is responsible for creating
 * environments for the target graph and actually executing the build, taking
 * into account the `formatter` and `dryRun` parameters passed in.
 */
class Executor {
protected:
    std::shared_ptr<Formatter> _formatter;
    bool                       _dryRun;

protected:
    Executor(std::shared_ptr<Formatter> const &formatter, bool dryRun);

public:
    virtual ~Executor();

public:
    /*
     * Abstract build method. Override to implement the build.
     */
    virtual bool build(
        Build::Environment const &buildEnvironment,
        Build::Context const &buildContext,
        DirectedGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph) = 0;
};

}
}

#endif // !__pbxbuild_Action_Executor_h
