/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_BuildContext_h
#define __pbxbuild_BuildContext_h

#include <pbxbuild/Base.h>
#include <pbxbuild/WorkspaceContext.h>
#include <pbxbuild/BuildEnvironment.h>
#include <pbxbuild/TargetEnvironment.h>

namespace pbxbuild {

class BuildContext {
private:
    std::shared_ptr<WorkspaceContext> _workspaceContext;
    xcscheme::XC::Scheme::shared_ptr  _scheme;
    std::string                       _action;
    std::string                       _configuration;
    bool                              _defaultConfiguration;
    std::vector<pbxsetting::Level>    _overrideLevels;

private:
    std::shared_ptr<std::unordered_map<pbxproj::PBX::Target::shared_ptr, pbxbuild::TargetEnvironment>> _targetEnvironments;

private:
    BuildContext(
        std::shared_ptr<WorkspaceContext> const &workspaceContext,
        xcscheme::XC::Scheme::shared_ptr const &scheme,
        std::string const &action,
        std::string const &configuration,
        bool defaultConfiguration,
        std::vector<pbxsetting::Level> const &overrideLevels
    );

public:
    std::shared_ptr<WorkspaceContext> const &workspaceContext() const
    { return _workspaceContext; }
    xcscheme::XC::Scheme::shared_ptr const &scheme() const
    { return _scheme; }

public:
    std::string const &action() const
    { return _action; }
    std::string const &configuration() const
    { return _configuration; }
    bool defaultConfiguration() const
    { return _defaultConfiguration; }

public:
    std::vector<pbxsetting::Level> const &overrideLevels() const
    { return _overrideLevels; }

public:
    pbxsetting::Level baseSettings(void) const;
    pbxsetting::Level actionSettings(void) const;

public:
    std::unique_ptr<pbxbuild::TargetEnvironment>
    targetEnvironment(BuildEnvironment const &buildEnvironment, pbxproj::PBX::Target::shared_ptr const &target) const;

public:
    pbxproj::PBX::Target::shared_ptr
    resolveTargetIdentifier(pbxproj::PBX::Project::shared_ptr const &project, std::string const &identifier) const;
    std::unique_ptr<std::pair<pbxproj::PBX::Target::shared_ptr, pbxproj::PBX::FileReference::shared_ptr>>
    resolveProductIdentifier(pbxproj::PBX::Project::shared_ptr const &project, std::string const &identifier) const;

public:
    static BuildContext
    Create(
        WorkspaceContext const &workspaceContext,
        xcscheme::XC::Scheme::shared_ptr const &scheme,
        std::string const &action,
        std::string const &configuration,
        bool defaultConfiguration,
        std::vector<pbxsetting::Level> const &overrideLevels
    );
};

}

#endif // !__pbxbuild_BuildContext_h
