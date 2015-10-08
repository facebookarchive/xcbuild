// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcdriver/ListAction.h>
#include <xcdriver/Action.h>
#include <xcdriver/Options.h>

using xcdriver::ListAction;
using xcdriver::Options;
using libutil::FSUtil;

ListAction::
ListAction()
{
}

ListAction::
~ListAction()
{
}

int ListAction::
Run(Options const &options)
{
    std::unique_ptr<pbxbuild::WorkspaceContext> context = Action::CreateWorkspace(options);
    if (context == nullptr) {
        return -1;
    }

    if (context->workspace() != nullptr) {
        xcworkspace::XC::Workspace::shared_ptr const &workspace = context->workspace();
        std::vector<xcscheme::XC::Scheme::shared_ptr> schemes = context->schemes();

        printf("Information about workspace \"%s\":\n", workspace->name().c_str());

        if (schemes.empty()) {
            printf("\n%4sThis workspace contains no scheme.\n", "");
        } else {
            std::sort(schemes.begin(), schemes.end(), [](xcscheme::XC::Scheme::shared_ptr const &a, xcscheme::XC::Scheme::shared_ptr const &b) -> bool {
                return ::strcasecmp(a->name().c_str(), b->name().c_str()) < 0;
            });

            auto I = std::unique(schemes.begin(), schemes.end(), [](xcscheme::XC::Scheme::shared_ptr const &a, xcscheme::XC::Scheme::shared_ptr const &b) -> bool {
                return (a->path() == b->path());
            });
            schemes.resize(std::distance(schemes.begin(), I));

            printf("%4sSchemes:\n", "");
            for (auto scheme : schemes) {
                printf("%8s%s\n", "", scheme->name().c_str());
            }
            printf("\n");
        }
    } else {
        // TODO(grp): Assume there is always one project. There should be more, when nested projects are preloaded.
        pbxproj::PBX::Project::shared_ptr const &project = context->projects().begin()->second;

        printf("Information about project \"%s\":\n", project->name().c_str());

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
            for (auto config : *project->buildConfigurationList()) {
                printf("%8s%s\n", "", config->name().c_str());
            }
            printf("\n%4sIf no build configuration is specified and -scheme is not passed then \"%s\" is used.\n", "", project->buildConfigurationList()->defaultConfigurationName().c_str());
        } else {
            printf("%4sThis project contains no build configurations.\n", "");
        }
        printf("\n");

        xcscheme::SchemeGroup::shared_ptr group = xcscheme::SchemeGroup::Open(project->projectFile(), project->name());
        if (!group->schemes().empty()) {
            printf("%4sSchemes:\n", "");

            for (auto scheme : group->schemes()) {
                printf("%8s%s\n", "", scheme->name().c_str());
            }
        } else {
            printf("%4sThis project contains no scheme.\n", "");
        }
    }

    return 0;
}
