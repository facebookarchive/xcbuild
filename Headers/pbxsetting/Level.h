// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Level_h
#define __pbxsetting_Level_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Setting.h>
#include <pbxsetting/Value.h>

namespace pbxsetting {

class Level {
private:
    std::shared_ptr<std::vector<Setting>> _settings;

public:
    Level(std::vector<Setting> const &settings);
    ~Level();

public:
    std::vector<Setting> const &settings() const
    { return *_settings; }

public:
    std::pair<bool, Value>
    get(std::string const &setting, Condition const &condition) const;
};

}

#endif  // !__pbxsetting_Level_h
