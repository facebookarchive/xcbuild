// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Setting.h>
#include <pbxsetting/Condition.h>

using pbxsetting::Setting;
using pbxsetting::Condition;
using libutil::trim;

Setting::
Setting(std::string const &name, Condition const &condition, Value const &value) :
    _name(name),
    _condition(condition),
    _value(value)
{
}

Setting::
~Setting()
{
}

bool Setting::
match(std::string const &name, Condition const &condition)
{
    return _name == name && _condition.match(condition);
}

bool Setting::
match(std::string const &name)
{
    return _name == name;
}

Setting Setting::
Parse(std::string const &string)
{
    size_t equal = 0;
    size_t sqbo  = 0;
    size_t sqbc  = 0;
    size_t comma = std::string::npos;
    std::unordered_map<std::string, std::string> conditions;

    do {
        equal = string.find('=', sqbc);
        sqbo  = string.find('[', sqbo + 1);
        sqbc  = string.find(']', sqbo);

        if (sqbo < sqbc && equal > sqbo && equal < sqbc) {
            do {
                size_t start = (comma != std::string::npos && comma > sqbo ? comma : sqbo);
                comma = string.find(',', start + 1);
                equal = string.find('=', start + 1);
                size_t end   = (comma != std::string::npos && comma < sqbc ? comma : sqbc);

                std::string key   = string.substr(start + 1, equal - start - 1);
                std::string value = string.substr(equal + 1, end - equal - 1);
                conditions.insert({ key, value });
            } while (comma != std::string::npos && comma < sqbc);
        }
    } while (sqbo < sqbc && equal > sqbo && equal < sqbc);

    sqbo = string.find('[');

    std::string key = string.substr(0, sqbo != std::string::npos ? sqbo : equal);
    std::string value = string.substr(equal + 1);

    trim(key);
    trim(value);

    return Setting(key, Condition(conditions), Value::Parse(value));
}

Setting Setting::
Parse(std::string const &key, std::string const &value)
{
    return Parse(key + "=" + value);
}
