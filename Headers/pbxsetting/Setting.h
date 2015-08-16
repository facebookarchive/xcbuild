// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Setting_h
#define __pbxsetting_Setting_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>

namespace pbxsetting {

class Setting {
private:
    std::string _name;
    Condition _condition;
    std::string _value;

public:
    Setting(std::string const &name, Condition const &condition, std::string const &value);
    ~Setting();

public:
    friend class std::hash<Setting>;

public:
    std::string const &
    name() const { return _name; }
    Condition const &
    condition() const { return _condition; }
    std::string const &
    value() const { return _value; }

public:
    bool
    match(std::string const &name, Condition const &condition);
    bool
    match(std::string const &name);

public:
    static Setting
    Parse(std::string const &string);
    static Setting
    Parse(std::string const &key, std::string const &value);
};

}

namespace std {
template<>
struct hash<pbxsetting::Setting> {
    size_t operator()(pbxsetting::Setting const &setting) const {
        return std::hash<std::string>()(setting._name) ^ std::hash<pbxsetting::Condition>()(setting._condition) ^ std::hash<std::string>()(setting._value);
    }
};
}

#endif  // !__pbxsetting_Setting_h
