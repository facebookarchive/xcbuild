/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Tool/ScriptResolver.h>
#include <pbxbuild/Tool/ToolContext.h>

using pbxbuild::Phase::LegacyTargetResolver;
using pbxbuild::Tool::ScriptResolver;
using pbxbuild::Tool::ToolContext;

LegacyTargetResolver::
LegacyTargetResolver(pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget) :
    _legacyTarget(legacyTarget)
{
}

LegacyTargetResolver::
~LegacyTargetResolver()
{
}

bool LegacyTargetResolver::
resolve(pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment, ToolContext *toolContext)
{
    std::unique_ptr<ScriptResolver> scriptResolver = ScriptResolver::Create(phaseEnvironment);
    if (scriptResolver == nullptr) {
        return false;
    }

    std::string const &workingDirectory = phaseEnvironment.targetEnvironment().workingDirectory();
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();

    scriptResolver->resolve(toolContext, environment, _legacyTarget);
    return true;
}
