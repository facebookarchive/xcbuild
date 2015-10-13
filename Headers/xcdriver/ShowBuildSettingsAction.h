// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcdriver_ShowBuildSettingsAction_h
#define __xcdriver_ShowBuildSettingsAction_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options;

class ShowBuildSettingsAction {
private:
    ShowBuildSettingsAction();
    ~ShowBuildSettingsAction();

public:
    static int
    Run(Options const &options);
};

}

#endif // !__xcdriver_ShowBuildSettingsAction_h
