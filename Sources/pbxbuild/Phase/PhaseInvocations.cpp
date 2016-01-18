/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Phase/PhaseInvocations.h>
#include <pbxbuild/Phase/Environment.h>
#include <pbxbuild/Phase/Context.h>
#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/ProductTypeResolver.h>
#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/ShellScriptResolver.h>
#include <pbxbuild/Tool/Context.h>

namespace Phase = pbxbuild::Phase;
namespace Tool = pbxbuild::Tool;
namespace Target = pbxbuild::Target;

Phase::PhaseInvocations::
PhaseInvocations(std::vector<Tool::Invocation const> const &invocations) :
    _invocations(invocations)
{
}

Phase::PhaseInvocations::
~PhaseInvocations()
{
}

Phase::PhaseInvocations Phase::PhaseInvocations::
Create(Phase::Environment const &phaseEnvironment, pbxproj::PBX::Target::shared_ptr const &target)
{
    Target::Environment const &targetEnvironment = phaseEnvironment.targetEnvironment();
    std::string const &workingDirectory = targetEnvironment.workingDirectory();

    /* Create the tool context for building. */
    Tool::SearchPaths searchPaths = Tool::SearchPaths::Create(targetEnvironment.environment(), workingDirectory);
    Tool::Context toolContext = Tool::Context(workingDirectory, searchPaths);

    Phase::Context phaseContext(toolContext);

    /* Filter build phases to ones appropriate for this target. */
    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> buildPhases;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Check buildActionMask against buildContext.action.
        // TODO(grp): Check runOnlyForDeploymentPostprocessing.
        buildPhases.push_back(buildPhase);
    }

    pbxproj::PBX::SourcesBuildPhase::shared_ptr sourcesPhase = nullptr;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        if (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeSources) {
            auto BP = std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase);
            sourcesPhase = BP;

            Phase::SourcesResolver sourcesResolver = Phase::SourcesResolver(BP);
            if (!sourcesResolver.resolve(phaseEnvironment, &phaseContext)) {
                fprintf(stderr, "error: unable to resolve sources\n");
            }
            break;
        }
    }

    if (sourcesPhase != nullptr && !sourcesPhase->files().empty()) {
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
        if (!link.resolve(phaseEnvironment, &phaseContext)) {
            fprintf(stderr, "error: unable to resolve linking\n");
        }
    }

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeShellScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ShellScriptBuildPhase> (buildPhase);

                Phase::ShellScriptResolver shellScript = Phase::ShellScriptResolver(BP);
                if (!shellScript.resolve(phaseEnvironment, &phaseContext)) {
                    fprintf(stderr, "error: unable to resolve shell script\n");
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeCopyFiles: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::CopyFilesBuildPhase> (buildPhase);

                Phase::CopyFilesResolver copyFiles = Phase::CopyFilesResolver(BP);
                if (!copyFiles.resolve(phaseEnvironment, &phaseContext)) {
                    fprintf(stderr, "error: unable to resolve copy files\n");
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeHeaders: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::HeadersBuildPhase> (buildPhase);

                Phase::HeadersResolver headers = Phase::HeadersResolver(BP);
                if (!headers.resolve(phaseEnvironment, &phaseContext)) {
                    fprintf(stderr, "error: unable to resolve headers\n");
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeResources: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ResourcesBuildPhase> (buildPhase);

                Phase::ResourcesResolver resources = Phase::ResourcesResolver(BP);
                if (!resources.resolve(phaseEnvironment, &phaseContext)) {
                    fprintf(stderr, "error: unable to resolve resources\n");
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

    /*
     * Add target-level invocations. Note these must come last as they use the context values set
     * by tools added above for the various build phases. For example, the final 'touch' must depend
     * on all previous invocations that created the output bundle.
     */
    switch (target->type()) {
        case pbxproj::PBX::Target::kTypeNative: {
            /*
             * Various product types have special (post-)processing. For example, bundles have
             * have info plist processing and applications additionally have a validation step.
             */
            if (pbxspec::PBX::ProductType::shared_ptr const &PT = phaseEnvironment.targetEnvironment().productType()) {
                Phase::ProductTypeResolver productType = Phase::ProductTypeResolver(PT);
                if (!productType.resolve(phaseEnvironment, &phaseContext)) {
                    fprintf(stderr, "error: unable to resolve product type\n");
                }
            }
            break;
        }
        case pbxproj::PBX::Target::kTypeLegacy: {
            /*
             * Run the script to build the legacy target.
             */
            pbxproj::PBX::LegacyTarget::shared_ptr LT = std::static_pointer_cast<pbxproj::PBX::LegacyTarget>(target);

            Phase::LegacyTargetResolver legacyScript = Phase::LegacyTargetResolver(LT);
            if (!legacyScript.resolve(phaseEnvironment, &phaseContext)) {
                fprintf(stderr, "error: unable to resolve legacy script\n");
            }

            break;
        }
        case pbxproj::PBX::Target::kTypeAggregate:
            break;
    }

    return Phase::PhaseInvocations(phaseContext.toolContext().invocations());
}

