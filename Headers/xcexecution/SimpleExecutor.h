/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcexecution_SimpleExecutor_h
#define __xcexecution_SimpleExecutor_h

#include <xcexecution/Executor.h>
#include <builtin/Registry.h>

namespace xcexecution {

/*
 * Simple executor that simply runs invocations in sequence. Advanced features
 * like incremental builds, dependency info, and such are not supported.
 */
class SimpleExecutor : public Executor {
private:
    builtin::Registry _builtins;

public:
    SimpleExecutor(std::shared_ptr<Formatter> const &formatter, bool dryRun, builtin::Registry const &builtins);
    ~SimpleExecutor();

public:
    virtual bool build(
        pbxbuild::Build::Environment const &buildEnvironment,
        pbxbuild::Build::Context const &buildContext,
        pbxbuild::DirectedGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph);

private:
    bool writeAuxiliaryFiles(
        pbxproj::PBX::Target::shared_ptr const &target,
        pbxbuild::Target::Environment const &targetEnvironment,
        std::vector<pbxbuild::Tool::Invocation const> const &invocations);
    std::pair<bool, std::vector<pbxbuild::Tool::Invocation const>> performInvocations(
        pbxproj::PBX::Target::shared_ptr const &target,
        pbxbuild::Target::Environment const &targetEnvironment,
        std::vector<pbxbuild::Tool::Invocation const> const &orderedInvocations,
        bool createProductStructure);
    std::pair<bool, std::vector<pbxbuild::Tool::Invocation const>> buildTarget(
        pbxproj::PBX::Target::shared_ptr const &target,
        pbxbuild::Target::Environment const &targetEnvironment,
        std::vector<pbxbuild::Tool::Invocation const> const &invocations);

public:
    static std::unique_ptr<SimpleExecutor>
    Create(std::shared_ptr<Formatter> const &formatter, bool dryRun, builtin::Registry const &builtins);
};

}

#endif // !__xcexecution_SimpleExecutor_h
