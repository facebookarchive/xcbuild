// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Setting_h
#define __pbxsetting_Setting_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Value.h>

namespace pbxsetting {

class Setting {
private:
    std::string _name;
    Condition _condition;
    Value _value;

public:
    Setting(std::string const &name, Condition const &condition, Value const &value);
    ~Setting();

public:
    std::string const &
    name() const { return _name; }
    Condition const &
    condition() const { return _condition; }
    Value const &
    value() const { return _value; }

public:
    bool
    match(std::string const &name, Condition const &condition) const;

public:
    static Setting
    Create(std::string const &key, Value const &value);

public:
    static Setting
    Parse(std::string const &string);
    static Setting
    Parse(std::string const &key, std::string const &value);
};

}

#endif  // !__pbxsetting_Setting_h
