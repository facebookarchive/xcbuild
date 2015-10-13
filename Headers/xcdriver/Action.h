// Copyright 2013-present Facebook. All Rights Reserved.

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
