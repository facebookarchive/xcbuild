/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/ScriptResolver.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;

Phase::LegacyTargetResolver::
LegacyTargetResolver(pbxproj::PBX::LegacyTarget::shared_ptr const &legacyTarget) :
    _legacyTarget(legacyTarget)
{
}

Phase::LegacyTargetResolver::
~LegacyTargetResolver()
{
}

bool Phase::LegacyTargetResolver::
resolve(Phase::PhaseEnvironment const &phaseEnvironment, Phase::PhaseContext *phaseContext)
{
    Tool::ScriptResolver const *scriptResolver = phaseContext->scriptResolver(phaseEnvironment);
    if (scriptResolver == nullptr) {
        return false;
    }

    std::string const &workingDirectory = phaseEnvironment.targetEnvironment().workingDirectory();
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();

    scriptResolver->resolve(&phaseContext->toolContext(), environment, _legacyTarget);
    return true;
}
