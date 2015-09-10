// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Type_h
#define __pbxsetting_Type_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Level.h>
#include <list>

namespace pbxsetting {

class Type {
private:
    Type();
    ~Type();

public:
    static bool
    ParseBoolean(std::string const &value);
    static std::vector<std::string>
    ParseList(std::string const &value);
};

}

#endif  // !__pbxsetting_Type_h
