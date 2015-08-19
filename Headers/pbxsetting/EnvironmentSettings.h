// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_EnvironmentSettings_h
#define __pbxsetting_EnvironmentSettings_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Setting.h>

namespace pbxsetting {

class EnvironmentSettings {
private:
    EnvironmentSettings();
    ~EnvironmentSettings();

public:
    static Level
    Default(void);
};

}

#endif  // !__pbxsetting_Environment_h
