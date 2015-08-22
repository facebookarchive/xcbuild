// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Environment_h
#define __pbxsetting_Environment_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Level.h>

namespace pbxsetting {

class Environment {
private:
    std::vector<Level> _assignment;
    std::vector<Level> _inheritance;

public:
    Environment(std::vector<Level> const &assignment, std::vector<Level> const &inheritance);
    ~Environment();

public:
    std::vector<Level> const &assignment() const
    { return _assignment; }
    std::vector<Level> const &inheritance() const
    { return _inheritance; }

public:
    std::string
    expand(Value const &value, Condition const &condition) const;
    std::string
    expand(Value const &value) const;

public:
    std::string
    resolve(std::string const &setting, Condition const &condition) const;
    std::string
    resolve(std::string const &setting) const;

public:
    std::unordered_map<std::string, std::string>
    computeValues(Condition const &condition) const;
};

}

#endif  // !__pbxsetting_Environment_h
