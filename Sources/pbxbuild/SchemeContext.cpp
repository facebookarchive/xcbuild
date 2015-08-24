// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/SchemeContext.h>

using pbxbuild::SchemeContext;

SchemeContext::
SchemeContext(xcscheme::XC::Scheme::shared_ptr const &scheme, xcworkspace::XC::Workspace::shared_ptr const &workspace, std::string const &action, std::string const &configuration) :
    _scheme(scheme),
    _workspace(workspace),
    _projects(std::make_shared<std::map<std::string, pbxproj::PBX::Project::shared_ptr>>()),
    _action(action),
    _configuration(configuration)
{
}

void SchemeContext::
registerProject(std::string const &path, pbxproj::PBX::Project::shared_ptr const &project) const
{
    (*_projects)[path] = project;
}

pbxsetting::Level SchemeContext::
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

pbxsetting::Level SchemeContext::
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

