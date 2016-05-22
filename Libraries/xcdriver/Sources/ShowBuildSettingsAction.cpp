/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/ShowBuildSettingsAction.h>
#include <xcdriver/Options.h>
#include <xcdriver/Action.h>
#include <libutil/DefaultFilesystem.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

using xcdriver::ShowBuildSettingsAction;
using xcdriver::Options;
using libutil::DefaultFilesystem;
using libutil::Filesystem;
using libutil::FSUtil;

ShowBuildSettingsAction::
ShowBuildSettingsAction()
{
}

ShowBuildSettingsAction::
~ShowBuildSettingsAction()
{
}

int ShowBuildSettingsAction::
Run(Options const &options)
{
    std::unique_ptr<Filesystem> filesystem = std::unique_ptr<Filesystem>(new DefaultFilesystem());

    if (!Action::VerifyBuildActions(options.actions())) {
        return -1;
    }

    ext::optional<pbxbuild::Build::Environment> buildEnvironment = pbxbuild::Build::Environment::Default(filesystem.get());
    if (!buildEnvironment) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    std::vector<pbxsetting::Level> overrideLevels = Action::CreateOverrideLevels(options, buildEnvironment->baseEnvironment());
    xcexecution::Parameters parameters = Action::CreateParameters(options, overrideLevels);

    ext::optional<pbxbuild::WorkspaceContext> workspaceContext = parameters.loadWorkspace(filesystem.get(), *buildEnvironment, FSUtil::GetCurrentDirectory());
    if (!workspaceContext) {
        return -1;
    }

    ext::optional<pbxbuild::Build::Context> buildContext = parameters.createBuildContext(*workspaceContext);
    if (!buildContext) {
        return -1;
    }

    ext::optional<pbxbuild::DirectedGraph<pbxproj::PBX::Target::shared_ptr>> graph = parameters.resolveDependencies(*buildEnvironment, *buildContext);
    if (!graph) {
        return -1;
    }

    ext::optional<std::vector<pbxproj::PBX::Target::shared_ptr>> targets = graph->ordered();
    if (!targets) {
        fprintf(stderr, "error: cycle detected in target dependencies\n");
        return -1;
    }

    for (pbxproj::PBX::Target::shared_ptr const &target : *targets) {
        ext::optional<pbxbuild::Target::Environment> targetEnvironment = buildContext->targetEnvironment(*buildEnvironment, target);
        if (!targetEnvironment) {
            fprintf(stderr, "error: couldn't create target environment\n");
            continue;
        }

        pbxsetting::Environment const &environment = targetEnvironment->environment();
        std::unordered_map<std::string, std::string> values = environment.computeValues(pbxsetting::Condition::Empty());
        std::map<std::string, std::string> orderedValues = std::map<std::string, std::string>(values.begin(), values.end());

        printf("Build settings for action %s and target %s:\n", buildContext->action().c_str(), target->name().c_str());
        for (auto const &value : orderedValues) {
            printf("    %s = %s\n", value.first.c_str(), value.second.c_str());
        }
        printf("\n");
    }

    return 0;
}
