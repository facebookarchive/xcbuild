/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/PhaseInvocations.h>
#include <pbxbuild/Phase/PhaseEnvironment.h>
#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/ShellScriptResolver.h>
#include <pbxbuild/Tool/ToolContext.h>
#include <pbxbuild/BuildGraph.h>

using pbxbuild::Phase::PhaseInvocations;
using pbxbuild::BuildGraph;
using pbxbuild::ToolInvocation;
using pbxbuild::Tool::ToolContext;

PhaseInvocations::
PhaseInvocations(std::vector<ToolInvocation> const &invocations) :
    _invocations(invocations)
{
}

PhaseInvocations::
~PhaseInvocations()
{
}

PhaseInvocations PhaseInvocations::
Create(PhaseEnvironment const &phaseEnvironment, pbxproj::PBX::Target::shared_ptr const &target)
{
    /* Create the tool context for building. */
    ToolContext toolContext = ToolContext(phaseEnvironment.targetEnvironment().workingDirectory());

    switch (target->type()) {
        case pbxproj::PBX::Target::kTypeAggregate:
        case pbxproj::PBX::Target::kTypeNative:
            break;
        case pbxproj::PBX::Target::kTypeLegacy:
            pbxproj::PBX::LegacyTarget::shared_ptr LT = std::static_pointer_cast <pbxproj::PBX::LegacyTarget> (target);
            auto legacyScript = pbxbuild::Phase::LegacyTargetResolver::Create(phaseEnvironment, LT);

            if (legacyScript != nullptr) {
                toolContext.invocations().insert(toolContext.invocations().end(), legacyScript->invocations().begin(), legacyScript->invocations().end());
            }

            break;
    }

    /* Filter build phases to ones appropriate for this target. */
    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> buildPhases;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Check buildActionMask against buildContext.action.
        // TODO(grp): Check runOnlyForDeploymentPostprocessing.
        buildPhases.push_back(buildPhase);
    }

    bool hasSourcesPhase = false;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        if (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeSources) {
            auto BP = std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase);
            hasSourcesPhase = true;

            Phase::SourcesResolver sourcesResolver = Phase::SourcesResolver(BP);
            if (!sourcesResolver.resolve(phaseEnvironment, &toolContext)) {
                fprintf(stderr, "error: unable to resolve sources\n");
            }
            break;
        }
    }

    if (hasSourcesPhase) {
        pbxproj::PBX::BuildPhase::shared_ptr buildPhase;
        pbxproj::PBX::FrameworksBuildPhase::shared_ptr frameworksPhase;

        auto it = std::find_if(buildPhases.begin(), buildPhases.end(), [](pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase) -> bool {
            return (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeFrameworks);
        });

        if (it == buildPhases.end()) {
            /* The target does not have an explicit frameworks phase, so synthesize an empty one. */
            frameworksPhase = std::make_shared <pbxproj::PBX::FrameworksBuildPhase> ();
            buildPhase = std::static_pointer_cast <pbxproj::PBX::BuildPhase> (frameworksPhase);
        } else {
            /* Use the frameworks phase from the target. */
            buildPhase = *it;
            frameworksPhase = std::static_pointer_cast <pbxproj::PBX::FrameworksBuildPhase> (buildPhase);
        }

        Phase::FrameworksResolver link = Phase::FrameworksResolver(frameworksPhase);
        if (!link.resolve(phaseEnvironment, &toolContext)) {
            fprintf(stderr, "error: unable to resolve linking\n");
        }
    }

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeShellScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ShellScriptBuildPhase> (buildPhase);
                auto shellScript = pbxbuild::Phase::ShellScriptResolver::Create(phaseEnvironment, BP);
                if (shellScript != nullptr) {
                    toolContext.invocations().insert(toolContext.invocations().end(), shellScript->invocations().begin(), shellScript->invocations().end());
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeCopyFiles: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::CopyFilesBuildPhase> (buildPhase);
                auto copyFiles = pbxbuild::Phase::CopyFilesResolver::Create(phaseEnvironment, BP);
                if (copyFiles != nullptr) {
                    toolContext.invocations().insert(toolContext.invocations().end(), copyFiles->invocations().begin(), copyFiles->invocations().end());
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeHeaders: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::HeadersBuildPhase> (buildPhase);
                auto headers = pbxbuild::Phase::HeadersResolver::Create(phaseEnvironment, BP);
                if (headers != nullptr) {
                    toolContext.invocations().insert(toolContext.invocations().end(), headers->invocations().begin(), headers->invocations().end());
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeResources: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ResourcesBuildPhase> (buildPhase);
                auto resources = pbxbuild::Phase::ResourcesResolver::Create(phaseEnvironment, BP);
                if (resources != nullptr) {
                    toolContext.invocations().insert(toolContext.invocations().end(), resources->invocations().begin(), resources->invocations().end());
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeAppleScript: {
                // TODO: Compile AppleScript
                auto BP = std::static_pointer_cast <pbxproj::PBX::AppleScriptBuildPhase> (buildPhase);
                break;
            }
            default: break;
        }
    }

    return PhaseInvocations(toolContext.invocations());
}

