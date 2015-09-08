// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Level.h>

using pbxsetting::Level;
using pbxsetting::Condition;
using pbxsetting::Setting;
using pbxsetting::Value;

Level::
Level(std::vector<Setting> const &settings) :
    _settings(std::make_shared<std::vector<Setting>>(settings))
{
}

Level::
~Level()
{
}

std::pair<bool, Value> Level::
get(std::string const &setting, Condition const &condition) const
{
    for (auto it = _settings->rbegin(); it != _settings->rend(); ++it) {
        if (it->match(setting, condition)) {
            return std::make_pair(true, it->value());
        }
    }

    return std::make_pair(false, Value::Empty());
}

