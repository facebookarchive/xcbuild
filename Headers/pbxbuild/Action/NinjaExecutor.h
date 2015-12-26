/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Action_NinjaExecutor_h
#define __pbxbuild_Action_NinjaExecutor_h

#include <pbxbuild/Action/Executor.h>
#include <builtin/builtin.h>

namespace ninja { class Writer; }

namespace pbxbuild {
namespace Action {

class NinjaExecutor : public Executor {
public:
    NinjaExecutor(std::shared_ptr<Formatter> const &formatter, bool dryRun);
    ~NinjaExecutor();

public:
    virtual bool build(
        Build::Environment const &buildEnvironment,
        Build::Context const &buildContext,
        DirectedGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph);

private:
    bool buildOutputDirectories(
        ninja::Writer *writer,
        std::vector<Tool::Invocation const> const &invocations,
        std::unordered_set<std::string> *seenDirectories);
    bool buildTargetAuxiliaryFiles(
        ninja::Writer *writer,
        pbxproj::PBX::Target::shared_ptr const &target,
        Target::Environment const &targetEnvironment,
        std::vector<Tool::Invocation const> const &invocations);
    bool buildTargetInvocations(
        pbxproj::PBX::Target::shared_ptr const &target,
        Target::Environment const &targetEnvironment,
        std::vector<Tool::Invocation const> const &invocations);

public:
    static std::unique_ptr<NinjaExecutor>
    Create(std::shared_ptr<Formatter> const &formatter, bool dryRun);
};

}
}

#endif // !__pbxbuild_Action_NinjaExecutor_h
