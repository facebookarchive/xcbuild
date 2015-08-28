// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/pbxsetting.h>
#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <xcscheme/xcscheme.h>
#include <xcworkspace/xcworkspace.h>
#include <pbxbuild/pbxbuild.h>

static std::unique_ptr<pbxbuild::FileTypeResolver>
ResolveBuildFile(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxsetting::Environment const &environment, pbxproj::PBX::BuildFile::shared_ptr const &buildFile)
{
    if (pbxproj::PBX::FileReference::shared_ptr const &fileReference = buildFile->fileReference()) {
        std::string path = environment.expand(fileReference->resolve());
        return pbxbuild::FileTypeResolver::Resolve(buildEnvironment.specManager(), fileReference, environment);
    } else if (pbxproj::PBX::ReferenceProxy::shared_ptr referenceProxy = buildFile->referenceProxy()) {
        pbxproj::PBX::ContainerItemProxy::shared_ptr proxy = referenceProxy->remoteRef();
        pbxproj::PBX::FileReference::shared_ptr containerReference = proxy->containerPortal();
        std::string containerPath = environment.expand(containerReference->resolve());

        auto remote = buildContext.resolveProductIdentifier(buildContext.project(containerPath), proxy->remoteGlobalIDString());
        if (remote == nullptr) {
            fprintf(stderr, "error: unable to find remote target product from proxied reference\n");
            return nullptr;
        }

        std::unique_ptr<pbxbuild::TargetEnvironment> remoteEnvironment = buildContext.targetEnvironment(buildEnvironment, remote->first);
        if (remoteEnvironment == nullptr) {
            fprintf(stderr, "error: unable to create target environment for remote target\n");
            return nullptr;
        }

        return pbxbuild::FileTypeResolver::Resolve(buildEnvironment.specManager(), remote->second, remoteEnvironment->environment());
    } else {
        fprintf(stderr, "error: unable to handle build file without file reference or proxy\n");
        return nullptr;
    }
}

static void
CompileFiles(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::TargetEnvironment const &targetEnvironment, pbxsetting::Environment const &environment, std::string const &variant, std::string const &arch, std::vector<pbxbuild::FileTypeResolver> const &files)
{
    for (pbxbuild::FileTypeResolver const &file : files) {
        pbxbuild::TargetBuildRules::BuildRule::shared_ptr buildRule = targetEnvironment.buildRules().resolve(file);

        std::string buildRuleDescription;
        if (buildRule != nullptr) {
            if (buildRule->tool() != nullptr) {
                buildRuleDescription = buildRule->tool()->identifier();
            } else if (!buildRule->script().empty()) {
                buildRuleDescription = "[custom script]";
            }
        } else {
            buildRuleDescription = "[no matching build rule]";
        }

        printf("\t\t\t%s (%s) -> %s\n", file.filePath().c_str(), file.fileType()->identifier().c_str(), buildRuleDescription.c_str());
    }
}

static void
LinkFiles(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::TargetEnvironment const &targetEnvironment, pbxsetting::Environment const &environment, std::string const &variant, std::string const &arch, std::vector<pbxbuild::FileTypeResolver> const &files)
{
    pbxspec::PBX::Linker::shared_ptr linker;
    if (environment.resolve("MACH_O_TYPE") == "staticlib") {
        linker = buildEnvironment.specManager()->linker("com.apple.pbx.linkers.libtool");
    } else {
        linker = buildEnvironment.specManager()->linker("com.apple.pbx.linkers.ld");
    }
    if (linker == nullptr) {
        fprintf(stderr, "error: couldn't get linker\n");
        return;
    }

    for (pbxbuild::FileTypeResolver const &file : files) {
        printf("\t\t\t%s (%s)\n", file.filePath().c_str(), file.fileType()->identifier().c_str());
    }
}

static void
BuildPhaseFiles(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target, pbxbuild::TargetEnvironment const &targetEnvironment, pbxproj::PBX::SourcesBuildPhase::shared_ptr const &buildPhase)
{
    for (std::string const &variant : targetEnvironment.variants()) {
        for (std::string const &arch : targetEnvironment.architectures()) {
            pbxsetting::Level currentLevel = pbxsetting::Level({
                pbxsetting::Setting::Parse("CURRENT_VARIANT", variant),
                pbxsetting::Setting::Parse("CURRENT_ARCH", arch),
            });
            std::vector<pbxsetting::Level> levels = targetEnvironment.environment().assignment();
            levels.push_back(currentLevel);
            pbxsetting::Environment currentEnvironment = pbxsetting::Environment(levels, levels);
            printf("\t\tArchitecture: %s; Variant: %s\n", arch.c_str(), variant.c_str());

            std::vector<pbxbuild::FileTypeResolver> files;
            files.reserve(buildPhase->files().size());
            for (pbxproj::PBX::BuildFile::shared_ptr const &buildFile : buildPhase->files()) {
                auto file = ResolveBuildFile(buildEnvironment, buildContext, currentEnvironment, buildFile);
                if (file == nullptr) {
                    continue;
                }
                files.push_back(*file);
            }

            switch (buildPhase->type()) {
                case pbxproj::PBX::BuildPhase::kTypeSources: {
                    CompileFiles(buildEnvironment, targetEnvironment, currentEnvironment, variant, arch, files);
                    break;
                }
                case pbxproj::PBX::BuildPhase::kTypeFrameworks: {
                    LinkFiles(buildEnvironment, targetEnvironment, currentEnvironment, variant, arch, files);
                    break;
                }
                default: {
                    assert(false);
                }
            }
        }
    }
}

static void
BuildTarget(pbxbuild::BuildEnvironment const &buildEnvironment, pbxbuild::BuildContext const &buildContext, pbxproj::PBX::Target::shared_ptr const &target)
{
    printf("Building Target: %s\n", target->name().c_str());
    std::unique_ptr<pbxbuild::TargetEnvironment> targetEnvironment = buildContext.targetEnvironment(buildEnvironment, target);
    if (targetEnvironment == nullptr) {
        fprintf(stderr, "error: couldn't create target environment\n");
        return;
    }

    for (pbxproj::PBX::BuildPhase::shared_ptr const &buildPhase : target->buildPhases()) {
        // TODO(grp): Check buildActionMask against buildContext.action.
        // TODO(grp): Check runOnlyForDeploymentPostprocessing.

        switch (buildPhase->type()) {
            case pbxproj::PBX::BuildPhase::kTypeHeaders: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::HeadersBuildPhase> (buildPhase);
                printf("\tTODO: Copy Headers\n");
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeSources: {
                printf("\tCompile Sources\n");
                BuildPhaseFiles(buildEnvironment, buildContext, target, *targetEnvironment, std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase));
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeResources: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ResourcesBuildPhase> (buildPhase);
                printf("\tTODO: Copy Resources\n");
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeCopyFiles: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::CopyFilesBuildPhase> (buildPhase);
                printf("\tTODO: Copy Files\n");
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeFrameworks: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::FrameworksBuildPhase> (buildPhase);
                printf("\tLink\n");
                BuildPhaseFiles(buildEnvironment, buildContext, target, *targetEnvironment, std::static_pointer_cast <pbxproj::PBX::SourcesBuildPhase> (buildPhase));
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeShellScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::ShellScriptBuildPhase> (buildPhase);
                printf("\tTODO: Run Shell Script\n");
                break;
            }
            case pbxproj::PBX::BuildPhase::kTypeAppleScript: {
                auto BP = std::static_pointer_cast <pbxproj::PBX::AppleScriptBuildPhase> (buildPhase);
                printf("\tTODO: Compile AppleScript\n");
                break;
            }
        }
    }
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

    pbxbuild::BuildContext context = pbxbuild::BuildContext::Workspace(
        workspace,
        scheme,
        argv[3],
        argv[4]
    );

    pbxbuild::DependencyResolver resolver = pbxbuild::DependencyResolver(*buildEnvironment);
    pbxbuild::BuildGraph graph = resolver.resolveDependencies(context);
    std::vector<pbxproj::PBX::Target::shared_ptr> targets = graph.ordered();

    for (pbxproj::PBX::Target::shared_ptr const &target : targets) {
        BuildTarget(*buildEnvironment, context, target);
    }
}
