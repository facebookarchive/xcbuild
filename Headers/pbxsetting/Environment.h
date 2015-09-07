// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Environment_h
#define __pbxsetting_Environment_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Condition.h>
#include <pbxsetting/Level.h>
#include <list>

namespace pbxsetting {

class Environment {
private:
    std::list<Level> _levels;

public:
    Environment(std::list<Level> const &levels);
    ~Environment();

public:
    std::list<Level> const &levels() const
    { return _levels; }

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
    std::vector<std::string>
    resolveList(std::string const &setting, Condition const &condition) const;
    std::vector<std::string>
    resolveList(std::string const &setting) const;

public:
    std::unordered_map<std::string, std::string>
    computeValues(Condition const &condition) const;

public:
    void insertFront(Level const &level);
    void insertBack(Level const &level);

public:
    static Environment const &Empty();
};

}

#endif  // !__pbxsetting_Environment_h
