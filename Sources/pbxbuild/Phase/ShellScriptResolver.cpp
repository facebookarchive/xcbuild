/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ShellScriptResolver.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Tool/ScriptResolver.h>

using pbxbuild::Phase::ShellScriptResolver;
using pbxbuild::Phase::PhaseContext;
using pbxbuild::Tool::ScriptResolver;

ShellScriptResolver::
ShellScriptResolver(pbxproj::PBX::ShellScriptBuildPhase::shared_ptr const &buildPhase) :
    _buildPhase(buildPhase)
{
}

ShellScriptResolver::
~ShellScriptResolver()
{
}

bool ShellScriptResolver::
resolve(pbxbuild::Phase::PhaseEnvironment const &phaseEnvironment, PhaseContext *phaseContext)
{
    ScriptResolver const *scriptResolver = phaseContext->scriptResolver(phaseEnvironment);
    if (scriptResolver == nullptr) {
        return false;
    }

    std::string const &workingDirectory = phaseEnvironment.targetEnvironment().workingDirectory();
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();

    scriptResolver->resolve(&phaseContext->toolContext(), environment, _buildPhase);
    return true;
}
