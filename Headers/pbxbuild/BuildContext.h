// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_BuildContext_h
#define __pbxbuild_BuildContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/TargetEnvironment.h>

namespace pbxbuild {

class BuildContext {
private:
    pbxproj::PBX::Project::shared_ptr       _project;
    xcworkspace::XC::Workspace::shared_ptr  _workspace;
    xcscheme::XC::Scheme::shared_ptr        _scheme;
    std::shared_ptr<std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr>> _projects;
    std::shared_ptr<std::unordered_map<pbxproj::PBX::Target::shared_ptr, pbxbuild::TargetEnvironment>> _targetEnvironments;

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
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects() const
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
    pbxproj::PBX::Project::shared_ptr
    project(std::string const &projectPath) const;
    std::unique_ptr<pbxbuild::TargetEnvironment>
    targetEnvironment(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target) const;

public:
    pbxproj::PBX::Target::shared_ptr
    resolveTargetIdentifier(pbxproj::PBX::Project::shared_ptr const &project, std::string const &identifier) const;
    std::unique_ptr<std::pair<pbxproj::PBX::Target::shared_ptr, pbxproj::PBX::FileReference::shared_ptr>>
    resolveProductIdentifier(pbxproj::PBX::Project::shared_ptr const &project, std::string const &identifier) const;

public:
    static BuildContext
    Workspace(xcworkspace::XC::Workspace::shared_ptr const &workspace, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration);
    static BuildContext
    Project(pbxproj::PBX::Project::shared_ptr const &project, xcscheme::XC::Scheme::shared_ptr const &scheme, std::string const &action, std::string const &configuration);
};

}

#endif // !__pbxbuild_BuildContext_h
