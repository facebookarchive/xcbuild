// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_BuildContext_h
#define __pbxbuild_BuildContext_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class BuildContext {
private:
    pbxproj::PBX::Project::shared_ptr       _project;
    xcworkspace::XC::Workspace::shared_ptr  _workspace;
    xcscheme::XC::Scheme::shared_ptr        _scheme;
    std::shared_ptr<std::map<std::string, pbxproj::PBX::Project::shared_ptr>> _projects;

private:
    std::string _action;
    std::string _configuration;

private:
    BuildContext(
        pbxproj::PBX::Project::shared_ptr const &project,
        xcworkspace::XC::Workspace::shared_ptr const &workspace,
        xcscheme::XC::Scheme::shared_ptr const &scheme,
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

public:
    static BuildContext
    Workspace(xcworkspace::XC::Workspace::shared_ptr const &workspace, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration);
    static BuildContext
    Project(pbxproj::PBX::Project::shared_ptr const &project, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration);
};

}

#endif // !__pbxbuild_BuildContext_h
