/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Tool/CopyResolver.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;

Phase::ResourcesResolver::
ResourcesResolver(pbxproj::PBX::ResourcesBuildPhase::shared_ptr const &buildPhase) :
    _buildPhase(buildPhase)
{
}

Phase::ResourcesResolver::
~ResourcesResolver()
{
}

bool Phase::ResourcesResolver::
resolve(Phase::Environment const &phaseEnvironment, Phase::Context *phaseContext)
{
    pbxsetting::Environment const &environment = phaseEnvironment.targetEnvironment().environment();
    pbxspec::Manager::shared_ptr const &specManager = phaseEnvironment.buildEnvironment().specManager();
    std::string resourcesDirectory = environment.resolve("BUILT_PRODUCTS_DIR") + "/" + environment.resolve("UNLOCALIZED_RESOURCES_FOLDER_PATH");

    std::vector<Phase::File> files = Phase::File::ResolveBuildFiles(phaseEnvironment, environment, _buildPhase->files());
    if (!phaseContext->resolveBuildFiles(phaseEnvironment, environment, _buildPhase, resourcesDirectory, files, Tool::CopyResolver::ToolIdentifier())) {
        return false;
    }

    return true;
}
