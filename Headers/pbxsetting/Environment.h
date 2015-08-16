// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Environment_h
#define __pbxsetting_Environment_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Level.h>

namespace pbxsetting {

class Environment {
private:
    std::vector<Level> _inheritance;
    std::vector<Level> _assignment;

public:
    std::string
    resolve(std::string const &setting);
    std::string
    resolve(std::string const &setting, Condition const &condition);

public:
    static std::unique_ptr<Environment>
    Create(std::vector<Level> const &levels);
};

}

#endif  // !__pbxsetting_Environment_h
