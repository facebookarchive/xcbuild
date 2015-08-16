// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsetting_Condition_h
#define __pbxsetting_Condition_h

#include <pbxsetting/Base.h>

namespace pbxsetting {

class Condition {
public:
private:
    std::unordered_map<std::string, std::string> _values;

public:
    Condition(std::unordered_map<std::string, std::string> const &values);
    ~Condition();

public:
    friend class std::hash<Condition>;

public:
    std::unordered_map<std::string, std::string> const &
    values() const { return _values; }

public:
    bool
    match(Condition const &condition);
};

}

namespace std {
template<>
struct hash<pbxsetting::Condition> {
    size_t operator()(pbxsetting::Condition const &condition) const {
        size_t hash = 0;
        for (auto pair : condition._values) {
            hash ^= std::hash<std::string>()(pair.first);
            hash ^= std::hash<std::string>()(pair.second);
        }
        return hash;
    }
};
}

#endif  // !__pbxsetting_Condition_h
