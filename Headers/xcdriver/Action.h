/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcdriver_Action_h
#define __xcdriver_Action_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class Action {
private:
    Action();
    ~Action();

public:
    enum Type {
        Build,
        ShowBuildSettings,
        List,
        Version,
        Usage,
        Help,
        License,
        CheckFirstLaunch,
        ShowSDKs,
        Find,
        ExportArchive,
        Localizations,
    };

public:
    static Type
    Determine(Options const &options);

public:
    static bool
    VerifyBuildActions(std::vector<std::string> const &actions);

public:
    static std::unique_ptr<pbxbuild::WorkspaceContext>
    CreateWorkspace(Options const &options);

public:
    static std::vector<pbxsetting::Level>
    CreateOverrideLevels(Options const &options, pbxsetting::Environment const &environment);

public:
    static std::unique_ptr<pbxbuild::BuildContext>
    CreateBuildContext(Options const &options, pbxbuild::WorkspaceContext const &workspaceContext, std::vector<pbxsetting::Level> const &overrideLevels);
};

}

#endif // !__xcdriver_Action_h
