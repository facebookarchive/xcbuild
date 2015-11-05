/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_WorkspaceContext_h
#define __pbxbuild_WorkspaceContext_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

class WorkspaceContext {
private:
    std::string                                   _basePath;
    std::string                                   _derivedDataName;
    xcworkspace::XC::Workspace::shared_ptr        _workspace;
    pbxproj::PBX::Project::shared_ptr             _project;
    std::vector<xcscheme::XC::Scheme::shared_ptr> _schemes;
    mutable std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> _projects;

public:
    WorkspaceContext(
        std::string const &basePath,
        std::string const &derivedDataName,
        xcworkspace::XC::Workspace::shared_ptr const &workspace,
        pbxproj::PBX::Project::shared_ptr const &project,
        std::vector<xcscheme::XC::Scheme::shared_ptr> const &schemes,
        std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects
    );
    ~WorkspaceContext();

public:
    std::string const &basePath() const
    { return _basePath; }
    std::string const &derivedDataName() const
    { return _derivedDataName; }

public:
    xcworkspace::XC::Workspace::shared_ptr const &workspace() const
    { return _workspace; }
    pbxproj::PBX::Project::shared_ptr const &project() const
    { return _project; }

public:
    std::vector<xcscheme::XC::Scheme::shared_ptr> const &schemes() const
    { return _schemes; }
    std::unordered_map<std::string, pbxproj::PBX::Project::shared_ptr> const &projects() const
    { return _projects; }

public:
    pbxproj::PBX::Project::shared_ptr
    project(std::string const &projectPath) const;
    xcscheme::XC::Scheme::shared_ptr
    scheme(std::string const &name) const;

public:
    static WorkspaceContext
    Workspace(xcworkspace::XC::Workspace::shared_ptr const &workspace);
    static WorkspaceContext
    Project(pbxproj::PBX::Project::shared_ptr const &project);
};

}

#endif // !__pbxbuild_WorkspaceContext_h
