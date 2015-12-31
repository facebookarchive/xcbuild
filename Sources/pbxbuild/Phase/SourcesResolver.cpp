/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Target/Environment.h>
#include <pbxbuild/Build/Environment.h>
#include <pbxbuild/Build/Context.h>
#include <pbxbuild/Tool/ClangResolver.h>
#include <pbxbuild/Tool/HeadermapResolver.h>
#include <pbxbuild/Tool/CompilationInfo.h>
#include <pbxbuild/Tool/HeadermapInfo.h>
#include <pbxbuild/Tool/PrecompiledHeaderInfo.h>
#include <pbxbuild/Tool/SearchPaths.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;

Phase::SourcesResolver::
SourcesResolver(pbxproj::PBX::SourcesBuildPhase::shared_ptr const &buildPhase) :
    _buildPhase(buildPhase)
{
}

Phase::SourcesResolver::
~SourcesResolver()
{
}

bool Phase::SourcesResolver::
resolve(Phase::Environment const &phaseEnvironment, Phase::Context *phaseContext)
{
    Build::Environment const &buildEnvironment = phaseEnvironment.buildEnvironment();
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();

    Tool::ClangResolver const *clangResolver = phaseContext->clangResolver(phaseEnvironment);
    if (clangResolver == nullptr) {
        return false;
    }

    std::unique_ptr<Tool::HeadermapResolver> headermapResolver = Tool::HeadermapResolver::Create(phaseEnvironment, clangResolver->compiler());
    if (headermapResolver == nullptr) {
        return false;
    }

    /* Populate the tool context with what's needed for compilation. */
    Tool::SearchPaths::Resolve(&phaseContext->toolContext(), targetEnvironment.environment());
    headermapResolver->resolve(&phaseContext->toolContext(), targetEnvironment.environment(), phaseEnvironment.target());

    std::vector<Phase::File> files = Phase::File::ResolveBuildFiles(phaseEnvironment, targetEnvironment.environment(), _buildPhase->files());

    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Environment currentEnvironment = targetEnvironment.environment();
            currentEnvironment.insertFront(Phase::Environment::VariantLevel(variant), false);
            currentEnvironment.insertFront(Phase::Environment::ArchitectureLevel(arch), false);

            std::string outputDirectory = currentEnvironment.expand(pbxsetting::Value::Parse("$(OBJECT_FILE_DIR_$(variant))/$(arch)"));

            if (!phaseContext->resolveBuildFiles(phaseEnvironment, currentEnvironment, _buildPhase, outputDirectory, files)) {
                return false;
            }
        }
    }

    return true;
}
