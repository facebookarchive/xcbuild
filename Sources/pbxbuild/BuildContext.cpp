// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildContext.h>

using pbxbuild::BuildContext;

BuildContext::
BuildContext(pbxproj::PBX::Project::shared_ptr const &project, xcworkspace::XC::Workspace::shared_ptr const &workspace, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration) :
    _project(project),
    _workspace(workspace),
    _scheme(scheme),
    _projects(std::make_shared<std::map<std::string, pbxproj::PBX::Project::shared_ptr>>()),
    _action(action),
    _configuration(configuration)
{
}

void BuildContext::
registerProject(std::string const &path, pbxproj::PBX::Project::shared_ptr const &project) const
{
    (*_projects)[path] = project;
}

pbxsetting::Level BuildContext::
actionSettings(void) const
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("ACTION", _action),
        pbxsetting::Setting::Parse("BUILD_COMPONENTS", "headers build"), // TODO(grp): Should depend on action?

        pbxsetting::Setting::Parse("CONFIGURATION", _configuration),

        pbxsetting::Setting::Parse("CURRENT_ARCH", "arm64"), // TODO(grp): Should intersect VALID_ARCHS and ARCHS?
        pbxsetting::Setting::Parse("CURRENT_VARIANT", "normal"),
    });
}

pbxsetting::Level BuildContext::
baseSettings(void) const
{
    return pbxsetting::Level({
        pbxsetting::Setting::Parse("CONFIGURATION_BUILD_DIR", "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),
        pbxsetting::Setting::Parse("CONFIGURATION_TEMP_DIR", "$(PROJECT_TEMP_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"),

        // TODO(grp): Replace these hardcoded values with real values.
        pbxsetting::Setting::Parse("SYMROOT", "$(DERIVED_DATA_DIR)/$(PROJECT_NAME)-cpmowfgmqamrjrfvwivglsgfzkff/Build/Products"),
        pbxsetting::Setting::Parse("OBJROOT", "$(DERIVED_DATA_DIR)/$(PROJECT_NAME)-cpmowfgmqamrjrfvwivglsgfzkff/Build/Intermediates"),
    });
}

BuildContext BuildContext::
Workspace(xcworkspace::XC::Workspace::shared_ptr const &workspace, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration)
{
    assert(workspace != nullptr);
    return BuildContext(nullptr, workspace, scheme, action, configuration);
}

BuildContext BuildContext::
Project(pbxproj::PBX::Project::shared_ptr const &project, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration)
{
    BuildContext buildContext = BuildContext(project, nullptr, scheme, action, configuration);
    buildContext.registerProject(project->projectFile(), project);
    return buildContext;
}
