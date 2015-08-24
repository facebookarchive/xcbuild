// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_SchemeContext_h
#define __pbxbuild_SchemeContext_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class SchemeContext {
private:
    xcscheme::XC::Scheme::shared_ptr        _scheme;
    xcworkspace::XC::Workspace::shared_ptr  _workspace;
    std::shared_ptr<std::map<std::string, pbxproj::PBX::Project::shared_ptr>> _projects;

private:
    std::string _action;
    std::string _configuration;

public:
    SchemeContext(
        xcscheme::XC::Scheme::shared_ptr const &scheme,
        xcworkspace::XC::Workspace::shared_ptr const &workspace,
        std::string const &action,
        std::string const &configuration
    );

public:
    xcscheme::XC::Scheme::shared_ptr const &scheme() const
    { return _scheme; }
    xcworkspace::XC::Workspace::shared_ptr const &workspace() const
    { return _workspace; }
    std::map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects() const
    { return *_projects; }

public:
    std::string const &action() const
    { return _action; }
    std::string const &configuration() const
    { return _configuration; }

public:
    pbxsetting::Level actionSettings(void) const;
    pbxsetting::Level baseSettings(void) const;

public:
    void registerProject(std::string const &path, pbxproj::PBX::Project::shared_ptr const &project) const;
};

}

#endif // !__pbxbuild_SchemeContext_h
