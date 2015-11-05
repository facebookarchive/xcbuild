/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_SimpleExecutor_h
#define __pbxbuild_SimpleExecutor_h

#include <pbxbuild/Build/Executor.h>

namespace pbxbuild {
namespace Build {

class SimpleExecutor : public Executor {
public:
    SimpleExecutor(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun);
    ~SimpleExecutor();

public:
    virtual void prepare();
    virtual void finish();

public:
    virtual bool buildTarget(
        pbxproj::PBX::Target::shared_ptr const &target,
        TargetEnvironment const &targetEnvironment,
        std::vector<pbxproj::PBX::BuildPhase::shared_ptr> const &orderedPhases,
        std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<ToolInvocation>> const &phaseInvocations
    );

public:
    static std::unique_ptr<SimpleExecutor>
    Create(BuildEnvironment const &buildEnvironment, BuildContext const &buildContext, std::shared_ptr<Formatter> const &formatter, bool dryRun);
};

}
}

#endif // !__pbxbuild_SimpleExecutor_h
