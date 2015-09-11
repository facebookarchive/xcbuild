// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <xcscheme/xcscheme.h>
#include <xcworkspace/xcworkspace.h>
#include <pbxbuild/pbxbuild.h>
#include <ios>
#include <fstream>

using libutil::FSUtil;
using libutil::Subprocess;

static std::vector<pbxproj::PBX::BuildPhase::shared_ptr>
SortBuildPhases(std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> phaseInvocations)
{
    std::unordered_map<std::string, pbxproj::PBX::BuildPhase::shared_ptr> outputToPhase;
    for (auto const &entry : phaseInvocations) {
        for (pbxbuild::ToolInvocation const &invocation : entry.second) {
            for (std::string const &output : invocation.outputs()) {
                outputToPhase.insert({ output, entry.first });
            }
        }
    }

    pbxbuild::BuildGraph<pbxproj::PBX::BuildPhase::shared_ptr> phaseGraph;
    for (auto const &entry : phaseInvocations) {
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

static std::vector<pbxbuild::ToolInvocation>
SortInvocations(std::vector<pbxbuild::ToolInvocation> invocations)
{
    std::unordered_map<std::string, pbxbuild::ToolInvocation const *> outputToInvocation;
    for (pbxbuild::ToolInvocation const &invocation : invocations) {
        for (std::string const &output : invocation.outputs()) {
            outputToInvocation.insert({ output, &invocation });
        }
    }

    pbxbuild::BuildGraph<pbxbuild::ToolInvocation const *> graph;
    for (pbxbuild::ToolInvocation const &invocation : invocations) {
        graph.insert(&invocation, { });

        for (std::string const &input : invocation.inputs()) {
            auto it = outputToInvocation.find(input);
            if (it != outputToInvocation.end()) {
                graph.insert(&invocation, { it->second });
            }
        }
    }

    std::vector<pbxbuild::ToolInvocation> result;
    for (pbxbuild::ToolInvocation const *invocation : graph.ordered()) {
        result.push_back(*invocation);
    }
    return result;
}

#define ANSI_STYLE_BOLD    "\033[1m"
#define ANSI_STYLE_NO_BOLD "\033[22m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static std::pair<bool, std::vector<pbxbuild::ToolInvocation>>
PerformBuild(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target, pbxbuild::TargetEnvironment const &targetEnvironment, std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> const &toolInvocations, std::vector<pbxproj::PBX::BuildPhase::shared_ptr> const &orderedPhases, bool execute = false)
{
    std::vector<pbxbuild::ToolInvocation> failures;

    printf("Write auxiliary files\n");
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : orderedPhases) {
        auto const entry = toolInvocations.find(buildPhase);
        for (pbxbuild::ToolInvocation const &invocation : entry->second) {
            for (std::string const &output : invocation.outputs()) {
                std::string directory = FSUtil::GetDirectoryName(output);
                if (!FSUtil::TestForDirectory(directory)) {
                    printf("/bin/mkdir -p %s\n", directory.c_str());

                    if (execute) {
                        Subprocess process;
                        if (!process.execute("/bin/mkdir", { "-p", directory }) || process.exitcode() != 0) {
                            return std::pair<bool, std::vector<pbxbuild::ToolInvocation>>(false, failures);
                        }
                    }
                }
            }

            for (pbxbuild::ToolInvocation::AuxiliaryFile const &auxiliaryFile : invocation.auxiliaryFiles()) {
                if (!FSUtil::TestForRead(auxiliaryFile.path())) {
                    Subprocess process;
                    if (!process.execute("/bin/mkdir", { "-p", FSUtil::GetDirectoryName(auxiliaryFile.path()) }) || process.exitcode() != 0) {
                        return std::pair<bool, std::vector<pbxbuild::ToolInvocation>>(false, failures);
                    }

                    printf("write-file %s\n", auxiliaryFile.path().c_str());

                    if (execute) {
                        std::ofstream out;
                        out.open(auxiliaryFile.path(), std::ios::out | std::ios::trunc | std::ios::binary);
                        out.write(auxiliaryFile.contents().data(), auxiliaryFile.contents().size() * sizeof(char));
                        out.close();
                    }

                    if (auxiliaryFile.executable() && !FSUtil::TestForExecute(auxiliaryFile.path())) {
                        printf("chmod 0755 %s\n", auxiliaryFile.path().c_str());

                        if (execute) {
                            Subprocess process;
                            if (!process.execute("/bin/chmod", { "0755", auxiliaryFile.path() }) || process.exitcode() != 0) {
                                return std::pair<bool, std::vector<pbxbuild::ToolInvocation>>(false, failures);
                            }
                        }
                    }
                }
            }
        }
    }
    printf("\n");

    printf("Create product structure\n");
    // TODO(grp): Create the product structure.
    printf("\n");

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : orderedPhases) {
        auto const entry = toolInvocations.find(buildPhase);
        std::vector<pbxbuild::ToolInvocation> orderedInvocations = SortInvocations(entry->second);

        for (pbxbuild::ToolInvocation const &invocation : orderedInvocations) {
            // TODO(grp): This should perhaps be a separate flag for a 'phony' invocation.
            if (invocation.executable().empty()) {
                continue;
            }

            if (execute) {
                for (std::string const &output : invocation.outputs()) {
                    std::string directory = FSUtil::GetDirectoryName(output);

                    if (!FSUtil::TestForDirectory(directory)) {
                        Subprocess process;
                        if (!process.execute("/bin/mkdir", { "-p", directory }) || process.exitcode() != 0) {
                            return std::pair<bool, std::vector<pbxbuild::ToolInvocation>>(false, failures);
                        }
                    }
                }
            }

            std::string message = invocation.logMessage();
            std::string::size_type space = message.find(' ');
            if (space != std::string::npos) {
                printf("%s%s%s", ANSI_STYLE_BOLD, message.substr(0, space).c_str(), ANSI_STYLE_NO_BOLD);
                message = message.substr(space);
            }
            printf("%s\n", message.c_str());

            printf("    cd %s\n", invocation.workingDirectory().c_str());

            for (std::pair<std::string, std::string> const &entry : invocation.environment()) {
                printf("    export %s=%s\n", entry.first.c_str(), entry.second.c_str());
            }

            std::string executable = invocation.executable();
            if (!FSUtil::IsAbsolutePath(executable)) {
                executable = FSUtil::FindExecutable(executable, targetEnvironment.sdk()->executablePaths());
            }
            printf("    %s", executable.c_str());

            for (std::string const &arg : invocation.arguments()) {
                printf(" %s", arg.c_str());
            }
            printf("\n");

            if (execute) {
                // TODO(grp): Change into the working directory.
                // TODO(grp): Apply environment variables.
                Subprocess process;
                if (!process.execute(executable, invocation.arguments(), invocation.environment(), invocation.workingDirectory()) || process.exitcode() != 0) {
                    printf("Command %s failed with exit code %d\n", executable.c_str(), process.exitcode());
                    failures.push_back(invocation);
                    return std::pair<bool, std::vector<pbxbuild::ToolInvocation>>(false, failures);
                }
            }

            printf("\n");
        }
    }

    return std::pair<bool, std::vector<pbxbuild::ToolInvocation>>(true, failures);
}

static std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>>
PhaseInvocations(pbxbuild::Phase::PhaseContext const &phaseContext, pbxproj::PBX::Target::shared_ptr const &target)
{
    // Filter build phases to ones appropriate for this target.
    std::vector<pbxproj::PBX::BuildPhase::shared_ptr> buildPhases;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Check buildActionMask against buildContext.action.
        // TODO(grp): Check runOnlyForDeploymentPostprocessing.
        buildPhases.push_back(buildPhase);
    }

    std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> toolInvocations;

    std::map<std::pair<std::string, std::string>, std::vector<pbxbuild::ToolInvocation>> sourcesInvocations;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        if (buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeSources) {
            auto BP = std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase);
            auto sources = pbxbuild::Phase::SourcesResolver::Create(phaseContext, BP);
            if (sources != nullptr) {
                sourcesInvocations.insert(sources->variantArchitectureInvocations().begin(), sources->variantArchitectureInvocations().end());
                toolInvocations.insert({ buildPhase, sources->invocations() });
            }
        }
    }

    bool foundSources = false;
    bool foundFrameworks = false;
    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        foundSources = (foundSources || buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeSources);
        foundFrameworks = (foundFrameworks || buildPhase->type() == pbxproj::PBX::BuildPhase::kTypeFrameworks);
    }
    if (foundSources && !foundFrameworks) {
        auto BP = std::make_shared <pbxproj::PBX::FrameworksBuildPhase> ();
        auto buildPhase = std::static_pointer_cast <pbxproj::PBX::BuildPhase> (BP);
        auto link = pbxbuild::Phase::FrameworksResolver::Create(phaseContext, BP, sourcesInvocations);
        if (link != nullptr) {
            toolInvocations.insert({ buildPhase, link->invocations() });
        }
    }

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeFrameworks: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::FrameworksBuildPhase> (buildPhase);
                auto link = pbxbuild::Phase::FrameworksResolver::Create(phaseContext, BP, sourcesInvocations);
                if (link != nullptr) {
                    toolInvocations.insert({ buildPhase, link->invocations() });
                }
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeShellScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ShellScriptBuildPhase> (buildPhase);
                auto shellScript = pbxbuild::Phase::ShellScriptResolver::Create(phaseContext, BP);
                if (shellScript != nullptr) {
                    toolInvocations.insert({ buildPhase, shellScript->invocations() });
                }
                break;
            }
            default: break;
        }
    }


    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : buildPhases) {
        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeShellScript:
            case pbxproj::PBX::BuildPhase::kTypeFrameworks:
            case pbxproj::PBX::BuildPhase::kTypeSources: {
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeHeaders: {
                // TODO: Copy Headers
                auto BP = std::static_pointer_cast <pbxproj::PBX::HeadersBuildPhase> (buildPhase);
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeResources: {
                // TODO: Copy Resources
                auto BP = std::static_pointer_cast <pbxproj::PBX::ResourcesBuildPhase> (buildPhase);
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeCopyFiles: {
                // TODO: Copy Files
                auto BP = std::static_pointer_cast <pbxproj::PBX::CopyFilesBuildPhase> (buildPhase);
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeAppleScript: {
                // TODO: Compile AppleScript
                auto BP = std::static_pointer_cast <pbxproj::PBX::AppleScriptBuildPhase> (buildPhase);
                break;
            }
        }
    }

    return toolInvocations;
}

int
main(int argc, char **argv)
{
    std::unique_ptr<pbxbuild::BuildEnvironment> buildEnvironment = pbxbuild::BuildEnvironment::Default();
    if (buildEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create build environment\n");
        return -1;
    }

    if (argc < 5) {
        printf("Usage: %s workspace scheme config action\n", argv[0]);
        return -1;
    }

    xcworkspace::XC::Workspace::shared_ptr workspace = xcworkspace::XC::Workspace::Open(argv[1]);
    if (workspace == nullptr) {
        fprintf(stderr, "failed opening workspace\n");
        return -1;
    }

    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(workspace->projectFile(), workspace->name());
    if (group == nullptr) {
        fprintf(stderr, "failed opening scheme\n");
        return -1;
    }

    xcscheme::XC::Scheme::shared_ptr scheme = nullptr;
    for (xcscheme::XC::Scheme::shared_ptr const &available : group->schemes()) {
        printf("scheme: %s\n", available->name().c_str());
        if (available->name() == argv[2]) {
            scheme = available;
            break;
        }
    }
    if (scheme == nullptr) {
        fprintf(stderr, "couldn't find scheme\n");
        return -1;
    }

    pbxbuild::BuildContext buildContext = pbxbuild::BuildContext::Workspace(
        workspace,
        scheme,
        argv[3],
        argv[4]
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    auto graph = resolver.resolveDependencies(buildContext);
    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    bool succeeded = true;
    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        printf("%s%s=== BUILD TARGET %s OF PROJECT %s WITH CONFIGURATION %s ===%s%s\n\n", ANSI_STYLE_BOLD, ANSI_COLOR_CYAN, target->name().c_str(), target->project()->name().c_str(), buildContext.configuration().c_str(), ANSI_STYLE_NO_BOLD, ANSI_COLOR_RESET);
        std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = buildContext.targetEnvironment(*buildEnvironment, target);
        if (targetEnvironment == nullptr) {
            fprintf(stderr, "error: couldn't create target environment\n");
            continue;
        }

        printf("Check dependencies\n\n");
        pbxbuild::Phase::PhaseContext phaseContext = pbxbuild::Phase::PhaseContext(*buildEnvironment, buildContext, target, *targetEnvironment);
        std::map<pbxproj::PBX::BuildPhase::shared_ptr, std::vector<pbxbuild::ToolInvocation>> phaseInvocations = PhaseInvocations(phaseContext, target);
        std::vector<pbxproj::PBX::BuildPhase::shared_ptr> orderedPhases = SortBuildPhases(phaseInvocations);

        auto result = PerformBuild(*buildEnvironment, buildContext, target, *targetEnvironment, phaseInvocations, orderedPhases, true);
        if (!result.first) {
            succeeded = false;
            printf("\n%s%s** BUILD FAILED **%s%s\n", ANSI_STYLE_BOLD, ANSI_COLOR_RED, ANSI_STYLE_NO_BOLD, ANSI_COLOR_RESET);
            printf("\n\nThe following build commands failed:\n");
            for (pbxbuild::ToolInvocation const &failure : result.second) {
                printf("    %s\n", failure.logMessage().c_str());
            }
            printf("(%zd failure%s)\n", result.second.size(), result.second.size() != 1 ? "s" : "");
            break;
        }
    }

    if (succeeded) {
        printf("\n%s%s** BUILD SUCCEEDED **%s%s\n", ANSI_STYLE_BOLD, ANSI_COLOR_GREEN, ANSI_STYLE_NO_BOLD, ANSI_COLOR_RESET);
    }
}
