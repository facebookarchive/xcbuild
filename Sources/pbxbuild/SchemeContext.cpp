// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/SchemeContext.h>

using pbxbuild::SchemeContext;

SchemeContext::
SchemeContext(std::string action, std::string configuration, xcscheme::XC::Scheme::shared_ptr scheme, xcworkspace::XC::Workspace::shared_ptr workspace, pbxsetting::Environment environment) :
    _action(action),
    _configuration(configuration),
    _scheme(scheme),
    _workspace(workspace),
    _environment(environment)
{
}

SchemeContext::shared_ptr SchemeContext::
Create(std::string action, std::string configuration, xcscheme::XC::Scheme::shared_ptr scheme, xcworkspace::XC::Workspace::shared_ptr workspace, pbxsetting::Environment environment)
{
    return std::make_shared<SchemeContext>(SchemeContext(action, configuration, scheme, workspace, environment));
}
