// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcdriver/Action.h>
#include <xcdriver/Options.h>

using xcdriver::Action;
using xcdriver::Options;
using libutil::FSUtil;

Action::
Action()
{
}

Action::
~Action()
{
}

static pbxproj::PBX::Project::shared_ptr
OpenProject(std::string const &projectPath, std::string const &directory)
{
    if (!projectPath.empty()) {
        return pbxproj::PBX::Project::Open(directory + "/" + projectPath);
    } else {
        bool multiple = false;
        std::string projectName;

        FSUtil::EnumerateDirectory(directory, "*.pbxproj", [&](std::string const &filename) -> bool {
            if (!projectName.empty()) {
                multiple = true;
            }

            projectName = filename;
            return true;
        });

        if (multiple) {
            fprintf(stderr, "error: multiple projects in directory\n");
            return nullptr;
        } else if (projectName.empty()) {
            fprintf(stderr, "error: no project found\n");
            return nullptr;
        } else {
            pbxproj::PBX::Project::shared_ptr project = pbxproj::PBX::Project::Open(directory + "/" + projectName);
            if (project == nullptr) {
                fprintf(stderr, "error: unable to open project '%s'\n", projectName.c_str());
            }
            return project;
        }
    }
}

std::unique_ptr<pbxbuild::WorkspaceContext> Action::
CreateWorkspace(Options const &options)
{
    if (!options.workspace().empty()) {
        xcworkspace::XC::Workspace::shared_ptr workspace = xcworkspace::XC::Workspace::Open(options.workspace());
        if (workspace == nullptr) {
            fprintf(stderr, "error: unable to open workspace '%s'\n", options.workspace().c_str());
            return nullptr;
        }

        pbxbuild::WorkspaceContext context = pbxbuild::WorkspaceContext::Workspace(workspace);
        return std::make_unique<pbxbuild::WorkspaceContext>(context);
    } else {
        pbxproj::PBX::Project::shared_ptr project = OpenProject(options.project(), ".");
        if (project == nullptr) {
            return nullptr;
        }

        pbxbuild::WorkspaceContext context = pbxbuild::WorkspaceContext::Project(project);
        return std::make_unique<pbxbuild::WorkspaceContext>(context);
    }
}

Action::Type Action::
Determine(Options const &options)
{
    if (options.version()) {
        return Version;
    } else if (options.usage()) {
        return Usage;
    } else if (options.help()) {
        return Help;
    } else if (options.license()) {
        return License;
    } else if (options.checkFirstLaunchStatus()) {
        return CheckFirstLaunch;
    } else if (options.showSDKs()) {
        return ShowSDKs;
    } else if (!options.findLibrary().empty() || !options.findExecutable().empty()) {
        return Find;
    } else if (options.exportArchive()) {
        return ExportArchive;
    } else if (options.exportLocalizations() || options.importLocalizations()) {
        return Localizations;
    } else if (options.list()) {
        return List;
    } else {
        return Build;
    }
}
