/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
    size_t           _offset;

public:
    Environment();
    ~Environment();

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

public:
    void insertFront(Level const &level, bool isDefault);
    void insertBack(Level const &level, bool isDefault);

public:
    void dump() const;

public:
    static Environment const &Empty();

private:
    struct InheritanceContext {
        bool valid;
        std::string setting;
        std::list<Level>::const_iterator it;
    };
    std::string resolveValue(Condition const &condition, Value const &value, InheritanceContext const &context) const;
    std::string resolveInheritance(Condition const &condition, InheritanceContext const &context) const;
    std::string resolveAssignment(Condition const &condition, std::string const &setting) const;
};

}

#endif  // !__pbxsetting_Environment_h
