/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <pbxproj/pbxproj.h>
#include <pbxsetting/Environment.h>
#include <pbxsetting/Setting.h>
#include <pbxsetting/XC/Config.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <xcscheme/xcscheme.h>
#include <libutil/DefaultFilesystem.h>
#include <libutil/Filesystem.h>
#include <process/DefaultContext.h>
#include <process/Context.h>
#include <process/DefaultUser.h>
#include <process/User.h>

#include <cstring>
#include <cerrno>
#include <set>

using namespace pbxproj;
using namespace libutil;

namespace {
void
CompleteDump(process::User const *user, Filesystem const *filesystem, PBX::Project::shared_ptr const &project)
{
    printf("Project File: %s\n", project->projectFile().c_str());
    printf("Base Path:    %s\n", project->basePath().c_str());
    printf("Name:         %s\n", project->name().c_str());

    printf("Schemes:\n");
    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(filesystem, user->userName(), project->basePath(), project->projectFile(), project->name());
    for (auto &I : group->schemes()) {
        printf("\t%s [%s]%s\n", I->name().c_str(),
                I->shared() ? "Shared" : I->owner().c_str(),
                I == group->defaultScheme() ? " (DEFAULT)" : "");
    }

    if (auto &DS = group->defaultScheme()) {
        if (auto &BA = DS->buildAction()) {
            for (auto &BAE : BA->buildActionEntries()) {
                auto const &BR = project->resolveBuildableReference(BAE->buildableReference()->blueprintIdentifier());
                printf("BAE:%s [%s]\n",
                        BAE->buildableReference()->blueprintIdentifier().c_str(),
                        BAE->buildableReference()->referencedContainer().c_str());
                if (BR) {
                    printf("%s\n", BR->isa().c_str());
                }
            }
        }
    }

    printf("Target List:\n");
    for (auto I : project->targets()) {
        printf("\t%s\n", I->name().c_str());
        printf("\t\tProduct Name = %s\n", I->productName().c_str());
        if (I->type() == PBX::Target::Type::Native) {
            auto NT = static_cast <PBX::NativeTarget const *> (I.get());
            printf("\t\tProduct Type = %s\n", NT->productType().c_str());
        }
        printf("\t\tConfigurations:\n");
        auto DCN = I->buildConfigurationList()->defaultConfigurationName();
        for (auto const &J : I->buildConfigurationList()->buildConfigurations()) {
            printf("\t\t\t%s%s\n", J->name().c_str(),
                    J->name() == DCN ?  " [Default]" : "");
        }

        printf("\t\tDependencies:\n");
        for (auto J : I->dependencies()) {
            if (J->target() != nullptr) {
                printf("\t\t\t%s\n", J->target()->name().c_str());
            } else if (J->targetProxy() != nullptr) {
                printf("\t\t\t%s [proxy; %s]\n", J->targetProxy()->remoteInfo().c_str(), J->targetProxy()->containerPortal()->resolve().raw().c_str());
            }
        }

        printf("\t\tBuild Phases:\n");
        for (auto J : I->buildPhases()) {
            switch (J->type()) {
                case PBX::BuildPhase::Type::Headers:
                    {
                        auto BP = static_cast <PBX::HeadersBuildPhase *> (J.get());
                        printf("\t\t\tHeaders Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::Sources:
                    {
                        auto BP = static_cast <PBX::SourcesBuildPhase *> (J.get());
                        printf("\t\t\tSources Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::Resources:
                    {
                        auto BP = static_cast <PBX::ResourcesBuildPhase *> (J.get());
                        printf("\t\t\tResources Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::CopyFiles:
                    {
                        auto BP = static_cast <PBX::CopyFilesBuildPhase *> (J.get());
                        printf("\t\t\tCopy Files Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tDestination Path                       = %s\n",
                                BP->dstPath().raw().c_str());
                        printf("\t\t\t\tDestination Subfolder Specifier        = %#x\n",
                                BP->dstSubfolderSpec());
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::Frameworks:
                    {
                        auto BP = static_cast <PBX::FrameworksBuildPhase *> (J.get());
                        printf("\t\t\tFrameworks Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::ShellScript:
                    {
                        auto BP = static_cast <PBX::ShellScriptBuildPhase *> (J.get());
                        printf("\t\t\tShell Script Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tName                                   = %s\n",
                                BP->name().c_str());
                        printf("\t\t\t\tShell Path                             = %s\n",
                                BP->shellPath().c_str());
                        printf("\t\t\t\tShell Script                           = %s\n",
                                BP->shellScript().c_str());
                        printf("\t\t\t\tInput Paths:\n");
                        for (auto K : BP->inputPaths()) {
                            printf("\t\t\t\t\t%s\n", K.raw().c_str());
                        }
                        printf("\t\t\t\tOutput Paths:\n");
                        for (auto K : BP->outputPaths()) {
                            printf("\t\t\t\t\t%s\n", K.raw().c_str());
                        }
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::AppleScript:
                    {
                        auto BP = static_cast <PBX::AppleScriptBuildPhase *> (J.get());
                        printf("\t\t\tApple Script Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tContext Name                           = %s\n",
                                BP->contextName().c_str());
                        printf("\t\t\t\tIs Shared Context                      = %s\n",
                                BP->isSharedContext() ? "YES" : "NO");
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
                case PBX::BuildPhase::Type::Rez:
                    {
                        auto BP = static_cast <PBX::RezBuildPhase *> (J.get());
                        printf("\t\t\tRez Build Phase:\n");
                        printf("\t\t\t\tBuild Action Mask                      = %#x\n",
                                BP->buildActionMask());
                        printf("\t\t\t\tRun Only For Deployment Postprocessing = %s\n",
                                BP->runOnlyForDeploymentPostprocessing() ? "YES" : "NO");
                        printf("\t\t\t\tFiles:\n");
                        for (auto K : BP->files()) {
                            if (auto FR = K->fileRef()) {
                                printf("\t\t\t\t\t%s\n", FR->resolve().raw().c_str());
                            }
                        }
                    }
                    break;
            }
        }
    }
}
}


int
main(int argc, char **argv)
{
    DefaultFilesystem filesystem = DefaultFilesystem();
    process::DefaultUser user = process::DefaultUser();

    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.xcodeproj\n", argv[0]);
        return -1;
    }

    auto project = PBX::Project::Open(&filesystem, argv[1]);
    if (!project) {
        fprintf(stderr, "error opening project (%s)\n",
                strerror(errno));
        return -1;
    }

    CompleteDump(&user, &filesystem, project);

    printf("Information about project \"%s\":\n",
            project->name().c_str());

    if (!project->targets().empty()) {
        printf("%4sTargets:\n", "");
        for (auto target : project->targets()) {
            printf("%8s%s\n", "", target->name().c_str());
        }
    } else {
        printf("%4sThis project contains no targets.\n", "");
    }
    printf("\n");

    if (project->buildConfigurationList()) {
        printf("%4sBuild Configurations:\n", "");
        for (auto const &config : project->buildConfigurationList()->buildConfigurations()) {
            printf("%8s%s\n", "", config->name().c_str());
        }
        printf("\n%4sIf no build configuration is specified and -scheme "
                "is not passed then \"%s\" is used.\n", "",
                project->buildConfigurationList()->defaultConfigurationName().c_str());
    } else {
        printf("%4sThis project contains no build configurations.\n", "");
    }
    printf("\n");

    xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(&filesystem, user.userName(), project->basePath(), project->projectFile(), project->name());
    if (!group->schemes().empty()) {
        printf("%4sSchemes:\n", "");
        for (auto scheme : group->schemes()) {
            printf("%8s%s\n", "", scheme->name().c_str());
        }
    } else {
        printf("%4sThis project contains no scheme.\n", "");
    }

    return 0;
}
