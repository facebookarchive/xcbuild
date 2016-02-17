/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcexecution_NinjaExecutor_h
#define __xcexecution_NinjaExecutor_h

#include <xcexecution/Executor.h>

namespace ninja { class Writer; }

namespace xcexecution {

/*
 * Concrete executor that generates Ninja files.
 */
class NinjaExecutor : public Executor {
public:
    NinjaExecutor(std::shared_ptr<Formatter> const &formatter, bool dryRun);
    ~NinjaExecutor();

public:
    virtual bool build(
        pbxbuild::Build::Environment const &buildEnvironment,
        pbxbuild::Build::Context const &buildContext,
        pbxbuild::DirectedGraph<pbxproj::PBX::Target::shared_ptr> const &targetGraph);

private:
    bool buildOutputDirectories(
        ninja::Writer *writer,
        std::vector<pbxbuild::Tool::Invocation> const &invocations,
        std::unordered_set<std::string> *seenDirectories);
    bool buildTargetAuxiliaryFiles(
        ninja::Writer *writer,
        pbxproj::PBX::Target::shared_ptr const &target,
        pbxbuild::Target::Environment const &targetEnvironment,
        std::vector<pbxbuild::Tool::Invocation> const &invocations);
    bool buildTargetInvocations(
        pbxproj::PBX::Target::shared_ptr const &target,
        pbxbuild::Target::Environment const &targetEnvironment,
        std::vector<pbxbuild::Tool::Invocation> const &invocations);

public:
    static std::unique_ptr<NinjaExecutor>
    Create(std::shared_ptr<Formatter> const &formatter, bool dryRun);
};

}

#endif // !__xcexecution_NinjaExecutor_h
