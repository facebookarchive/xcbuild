/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
        return pbxproj::PBX::Project::Open(projectPath);
    } else {
        bool multiple = false;
        std::string projectName;

        FSUtil::EnumerateDirectory(directory, "*.xcodeproj", [&](std::string const &filename) -> bool {
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
        return std::unique_ptr<pbxbuild::WorkspaceContext>(new pbxbuild::WorkspaceContext(context));
    } else {
        pbxproj::PBX::Project::shared_ptr project = OpenProject(options.project(), FSUtil::GetCurrentDirectory());
        if (project == nullptr) {
            return nullptr;
        }

        pbxbuild::WorkspaceContext context = pbxbuild::WorkspaceContext::Project(project);
        return std::unique_ptr<pbxbuild::WorkspaceContext>(new pbxbuild::WorkspaceContext(context));
    }
}

std::vector<pbxsetting::Level> Action::
CreateOverrideLevels(Options const &options, pbxsetting::Environment const &environment)
{
    std::vector<pbxsetting::Level> levels;

    std::vector<pbxsetting::Setting> settings;
    if (!options.sdk().empty()) {
        settings.push_back(pbxsetting::Setting::Create("SDKROOT", options.sdk()));
    }
    if (!options.arch().empty()) {
        settings.push_back(pbxsetting::Setting::Create("ARCHS", options.arch()));
    }
    levels.push_back(pbxsetting::Level(settings));

    levels.push_back(options.settings());

    if (!options.xcconfig().empty()) {
        pbxsetting::XC::Config::shared_ptr config = pbxsetting::XC::Config::Open(options.xcconfig(), environment);
        if (config == nullptr) {
            fprintf(stderr, "warning: unable to open xcconfig '%s'\n", options.xcconfig().c_str());
        } else {
            levels.push_back(config->level());
        }
    }

    if (getenv("XCODE_XCCONFIG_FILE")) {
        std::string path = getenv("XCODE_XCCONFIG_FILE");

        pbxsetting::XC::Config::shared_ptr config = pbxsetting::XC::Config::Open(path, environment);
        if (config == nullptr) {
            fprintf(stderr, "warning: unable to open xcconfig from environment '%s'\n", path.c_str());
        } else {
            levels.push_back(config->level());
        }
    }

    return levels;
}

std::unique_ptr<pbxbuild::Build::Context> Action::
CreateBuildContext(Options const &options, pbxbuild::WorkspaceContext const &workspaceContext, std::vector<pbxsetting::Level> const &overrideLevels)
{
    std::vector<std::string> actions = (!options.actions().empty() ? options.actions() : std::vector<std::string>({ "build" }));
    std::string action = actions.front(); // TODO(grp): Support multiple actions and skipUnavailableOptions.
    if (action != "build") {
        fprintf(stderr, "error: action '%s' is not implemented\n", action.c_str());
        return nullptr;
    }

    /* Find the scheme from the options. */
    xcscheme::XC::Scheme::shared_ptr scheme = nullptr;
    xcscheme::SchemeGroup::shared_ptr schemeGroup = nullptr;
    if (!options.scheme().empty()) {
        for (xcscheme::SchemeGroup::shared_ptr const &schemeGroup_ : workspaceContext.schemeGroups()) {
            if (xcscheme::XC::Scheme::shared_ptr scheme_ = schemeGroup_->scheme(options.scheme())) {
                scheme = scheme_;
                schemeGroup = schemeGroup_;
            }
        }
    }

    std::string configuration = options.configuration();
    bool defaultConfiguration = false;
    if (configuration.empty()) {
        if (scheme != nullptr) {
            configuration = scheme->buildAction()->buildConfiguration();
            if (configuration.empty()) {
                configuration = "Debug";
            }
        } else if (workspaceContext.project() != nullptr) {
            defaultConfiguration = true;
            configuration = workspaceContext.project()->buildConfigurationList()->defaultConfigurationName();
        }

        if (configuration.empty()) {
            fprintf(stderr, "error: unable to determine build configuration\n");
            return nullptr;
        }
    }

    return std::unique_ptr<pbxbuild::Build::Context>(new pbxbuild::Build::Context(
        workspaceContext,
        scheme,
        schemeGroup,
        action,
        configuration,
        defaultConfiguration,
        overrideLevels
    ));
}

bool Action::
VerifyBuildActions(std::vector<std::string> const &actions)
{
    for (std::string const &action : actions) {
        if (action != "build" &&
            action != "analyze" &&
            action != "archive" &&
            action != "test" &&
            action != "installsrc" &&
            action != "install" &&
            action != "clean") {
            fprintf(stderr, "error: unknown build action '%s'\n", action.c_str());
            return false;
        }
    }

    return true;
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
    } else if (options.showBuildSettings()) {
        return ShowBuildSettings;
    } else {
        return Build;
    }
}
