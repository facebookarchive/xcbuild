// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Setting.h>
#include <pbxsetting/Condition.h>

using pbxsetting::Setting;
using pbxsetting::Condition;
using libutil::trim;

bool
Setting::match(std::string const &name, Condition const &condition)
{
    return _name == name && _condition.match(condition);
}

bool
Setting::match(std::string const &name)
{
    return _name == name;
}

static Setting
Parse(std::string const &string)
{
    size_t equal;
    size_t sqbo;
    size_t sqbc;
    std::unordered_map<std::string, std::string> conditions;

    do {
        equal = string.find('=', sqbc);
        sqbo  = string.find('[', sqbo + 1);
        sqbc  = string.find(']', sqbo);

        if (sqbo < sqbc && equal > sqbo && equal < sqbc) {
            std::string key = string.substr(sqbo + 1, equal - sqbo - 1);
            std::string value = string.substr(equal + 1, sqbc - equal - 1);
            conditions.insert({ key, value });
        }
    } while (sqbo < sqbc && equal > sqbo && equal < sqbc);

    std::string key = string.substr(0, equal);
    std::string value = string.substr(equal + 1);

    return Setting(key, Condition(conditions), value);
}

static Setting
Parse(std::string const &key, std::string const &value)
{
    return Parse(key + "=" + value);
}
