/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcexecution/Parameters.h>

#include <pbxbuild/Build/DependencyResolver.h>
#include <libutil/FSUtil.h>

using xcexecution::Parameters;
using libutil::FSUtil;

Parameters::
Parameters(
    ext::optional<std::string> const &workspace,
    ext::optional<std::string> const &project,
    ext::optional<std::string> const &scheme,
    ext::optional<std::string> const &target,
    bool allTargets,
    std::vector<std::string> const &actions,
    ext::optional<std::string> const &configuration,
    std::vector<pbxsetting::Level> const &overrideLevels) :
    _workspace     (workspace),
    _project       (project),
    _scheme        (scheme),
    _target        (target),
    _allTargets    (allTargets),
    _actions       (actions),
    _configuration (configuration),
    _overrideLevels(overrideLevels)
{
}

static pbxproj::PBX::Project::shared_ptr
OpenProject(ext::optional<std::string> const &projectPath, std::string const &directory)
{
    if (projectPath) {
        return pbxproj::PBX::Project::Open(*projectPath);
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

ext::optional<pbxbuild::WorkspaceContext> Parameters::
loadWorkspace(pbxbuild::Build::Environment const &buildEnvironment, std::string const &workingDirectory)
{
    if (_workspace) {
        xcworkspace::XC::Workspace::shared_ptr workspace = xcworkspace::XC::Workspace::Open(*_workspace);
        if (workspace == nullptr) {
            fprintf(stderr, "error: unable to open workspace '%s'\n", _workspace->c_str());
            return ext::nullopt;
        }

        return pbxbuild::WorkspaceContext::Workspace(buildEnvironment.baseEnvironment(), workspace);
    } else {
        pbxproj::PBX::Project::shared_ptr project = OpenProject(_project, workingDirectory);
        if (project == nullptr) {
            return ext::nullopt;
        }

        return pbxbuild::WorkspaceContext::Project(buildEnvironment.baseEnvironment(), project);
    }
}

ext::optional<pbxbuild::Build::Context> Parameters::
createBuildContext(pbxbuild::WorkspaceContext const &workspaceContext)
{
    std::vector<std::string> actions = (!_actions.empty() ? _actions : std::vector<std::string>({ "build" }));
    std::string action = actions.front(); // TODO(grp): Support multiple actions and skipUnavailableOptions.
    if (action != "build") {
        fprintf(stderr, "error: action '%s' is not implemented\n", action.c_str());
        return ext::nullopt;
    }

    /* Find the scheme from the options. */
    xcscheme::XC::Scheme::shared_ptr scheme = nullptr;
    xcscheme::SchemeGroup::shared_ptr schemeGroup = nullptr;
    if (_scheme) {
        for (xcscheme::SchemeGroup::shared_ptr const &schemeGroup_ : workspaceContext.schemeGroups()) {
            if (xcscheme::XC::Scheme::shared_ptr scheme_ = schemeGroup_->scheme(*_scheme)) {
                scheme = scheme_;
                schemeGroup = schemeGroup_;
            }
        }

        if (scheme == nullptr || schemeGroup == nullptr) {
            fprintf(stderr, "error: unable to find scheme '%s'\n", _scheme->c_str());
            return ext::nullopt;
        }
    }

    std::string configuration;
    bool defaultConfiguration;

    if (_configuration) {
        defaultConfiguration = false;
        configuration = *_configuration;
    } else {
        defaultConfiguration = true;

        if (scheme != nullptr) {
            configuration = scheme->buildAction()->buildConfiguration();
            if (configuration.empty()) {
                configuration = "Debug";
            }
        } else if (workspaceContext.project() != nullptr) {
            defaultConfiguration = true;
            configuration = workspaceContext.project()->buildConfigurationList()->defaultConfigurationName();
        } else {
            fprintf(stderr, "error: a scheme is required to build a workspace\n");
            return ext::nullopt;
        }

        if (configuration.empty()) {
            fprintf(stderr, "error: unable to determine build configuration\n");
            return ext::nullopt;
        }
    }

    return pbxbuild::Build::Context(
        workspaceContext,
        scheme,
        schemeGroup,
        action,
        configuration,
        defaultConfiguration,
        _overrideLevels);
}

ext::optional<pbxbuild::DirectedGraph<pbxproj::PBX::Target::shared_ptr>> Parameters::
resolveDependencies(pbxbuild::Build::Environment const &buildEnvironment, pbxbuild::Build::Context const &buildContext)
{
    pbxbuild::Build::DependencyResolver resolver = pbxbuild::Build::DependencyResolver(buildEnvironment);

    if (buildContext.scheme() != nullptr) {
        return resolver.resolveSchemeDependencies(buildContext);
    } else if (buildContext.workspaceContext().project() != nullptr) {
        return resolver.resolveLegacyDependencies(buildContext, _allTargets, _target);
    } else {
        fprintf(stderr, "error: scheme is required for workspace\n");
        return ext::nullopt;
    }
}

