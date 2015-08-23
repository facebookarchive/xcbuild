// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_SchemeContext_h
#define __pbxbuild_SchemeContext_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class SchemeContext {
public:
    typedef std::shared_ptr <SchemeContext> shared_ptr;

private:
    std::string                                              _action;
    std::string                                              _configuration;
    xcscheme::XC::Scheme::shared_ptr                         _scheme;
    xcworkspace::XC::Workspace::shared_ptr                   _workspace;
    std::map<std::string, pbxproj::PBX::Project::shared_ptr> _projects;
    pbxsetting::Environment                                  _environment;

private:
    SchemeContext(std::string action, std::string configuration, xcscheme::XC::Scheme::shared_ptr scheme, xcworkspace::XC::Workspace::shared_ptr workspace, pbxsetting::Environment environment);

public:
    std::string const &action() const
    { return _action; }
    std::string const &configuration() const
    { return _configuration; }

public:
    xcscheme::XC::Scheme::shared_ptr const &scheme() const
    { return _scheme; }
    xcworkspace::XC::Workspace::shared_ptr const &workspace() const
    { return _workspace; }

public:
    std::map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects() const
    { return _projects; }
    std::map<std::string, pbxproj::PBX::Project::shared_ptr> &projects()
    { return _projects; }

public:
    pbxsetting::Environment const &environment() const
    { return _environment; }

public:
    static SchemeContext::shared_ptr
    Create(std::string action, std::string configuration, xcscheme::XC::Scheme::shared_ptr scheme, xcworkspace::XC::Workspace::shared_ptr workspace, pbxsetting::Environment environment);
};

}

#endif // !__pbxbuild_SchemeContext_h
