// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Level_h
#define __pbxsetting_Level_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Setting.h>

namespace pbxsetting {

class Level {
private:
    std::string _name;
    std::unordered_set<Setting> _settings;
};

}

#endif  // !__pbxsetting_Level_h
