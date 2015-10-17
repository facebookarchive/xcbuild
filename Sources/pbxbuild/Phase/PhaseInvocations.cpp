// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/Phase/PhaseInvocations.h>
#include <pbxbuild/Phase/PhaseContext.h>
#include <pbxbuild/Phase/CopyFilesResolver.h>
#include <pbxbuild/Phase/HeadersResolver.h>
#include <pbxbuild/Phase/LegacyTargetResolver.h>
#include <pbxbuild/Phase/ResourcesResolver.h>
#include <pbxbuild/Phase/FrameworksResolver.h>
#include <pbxbuild/Phase/SourcesResolver.h>
#include <pbxbuild/Phase/ShellScriptResolver.h>
#include <pbxbuild/BuildGraph.h>

using pbxbuild::Phase::PhaseInvocations;
using pbxbuild::BuildGraph;
using pbxbuild::ToolInvocation;

PhaseInvocations::
PhaseInvocations(std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<ToolInvocation>> const &invocations) :
    _invocations(invocations)
{
}

PhaseInvocations::
~PhaseInvocations()
{
}

std::vector<pbxproj::PBX::BuildPhase::shared_ptr> PhaseInvocations::
orderedPhases(void)
{
    std::unordered_map<std::string, pbxproj::PBX::BuildPhase::shared_ptr> outputToPhase;
    for (auto const &entry : _invocations) {
        for (pbxbuild::ToolInvocation const &invocation : entry.second) {
            for (std::string const &output : invocation.outputs()) {
                outputToPhase.insert({ output, entry.first });
            }
        }
    }

    pbxbuild::BuildGraph<pbxproj::PBX::BuildPhase::shared_ptr> phaseGraph;
    for (auto const &entry : _invocations) {
        phaseGraph.insert(entry.first, { });

        for (pbxbuild::ToolInvocation const &invocation : entry.second) {
            for (std::string const &input : invocation.inputs()) {
                auto it = outputToPhase.find(input);
                if (it != outputToPhase.end()) {
                    if (it->second != entry.first) {
                        phaseGraph.insert(entry.first, { it->second });
                    }
                }
            }
        }
    }

    return phaseGraph.ordered();
}

std::vector<ToolInvocation> PhaseInvocations::
phaseInvocations(pbxproj::PBX::BuildPhase::shared_ptr const &phase)
{
    return _invocations.find(phase)->second;
}

PhaseInvocations PhaseInvocations::
Create(PhaseContext const &phaseContext, pbxproj::PBX::Target::shared_ptr const &target)
{
    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> buildPhases;
    std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> toolInvocations;

    switch (target->type()) {
        case pbxproj::PBX::Target::kTypeAggregate:
        case pbxproj::PBX::Target::kTypeNative:
            break;
        case pbxproj::PBX::Target::kTypeLegacy:
            pbxproj::PBX::LegacyTarget::shared_ptr LT = std::static_pointer_cast <pbxproj::PBX::LegacyTarget> (target);
            auto legacyScript = pbxbuild::Phase::LegacyTargetResolver::Create(phaseContext, LT);

            if (legacyScript != nullptr) {
                toolInvocations.insert({ nullptr, legacyScript->invocations() });
            }

            break;
    }

    // Filter build phases to ones appropriate for this target.
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Check buildActionMask against buildContext.action.
        // TODO(grp): Check runOnlyForDeploymentPostprocessing.
        buildPhases.push_back(buildPhase);
    }

    std::unique_ptr<pbxbuild::Phase::SourcesResolver> sourcesResolver;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        if (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeSources) {
            auto BP = std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase);
            sourcesResolver = pbxbuild::Phase::SourcesResolver::Create(phaseContext, BP);

            if (sourcesResolver != nullptr) {
                toolInvocations.insert({ buildPhase, sourcesResolver->invocations() });
            }

            break;
        }
    }

    if (sourcesResolver != nullptr) {
        pbxproj::PBX::BuildPhase::shared_ptr buildPhase;
        pbxproj::PBX::FrameworksBuildPhase::shared_ptr frameworksPhase;

        auto it = std::find_if(buildPhases.begin(), buildPhases.end(), [](pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase) -> bool {
            return (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeFrameworks);
        });

        if (it == buildPhases.end()) {
            frameworksPhase = std::make_shared <pbxproj::PBX::FrameworksBuildPhase> ();
            buildPhase = std::static_pointer_cast <pbxproj::PBX::BuildPhase> (frameworksPhase);
        } else {
            buildPhase = *it;
            frameworksPhase = std::static_pointer_cast <pbxproj::PBX::FrameworksBuildPhase> (buildPhase);
        }

        auto link = pbxbuild::Phase::FrameworksResolver::Create(phaseContext, frameworksPhase, *sourcesResolver);
        if (link != nullptr) {
            toolInvocations.insert({ buildPhase, link->invocations() });
        }
    }

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeShellScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ShellScriptBuildPhase> (buildPhase);
                auto shellScript = pbxbuild::Phase::ShellScriptResolver::Create(phaseContext, BP);
                if (shellScript != nullptr) {
                    toolInvocations.insert({ buildPhase, shellScript->invocations() });
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeCopyFiles: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::CopyFilesBuildPhase> (buildPhase);
                auto copyFiles = pbxbuild::Phase::CopyFilesResolver::Create(phaseContext, BP);
                if (copyFiles != nullptr) {
                    toolInvocations.insert({ buildPhase, copyFiles->invocations() });
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeHeaders: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::HeadersBuildPhase> (buildPhase);
                auto headers = pbxbuild::Phase::HeadersResolver::Create(phaseContext, BP);
                if (headers != nullptr) {
                    toolInvocations.insert({ buildPhase, headers->invocations() });
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeResources: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ResourcesBuildPhase> (buildPhase);
                auto resources = pbxbuild::Phase::ResourcesResolver::Create(phaseContext, BP);
                if (resources != nullptr) {
                    toolInvocations.insert({ buildPhase, resources->invocations() });
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

    return PhaseInvocations(toolInvocations);
}

