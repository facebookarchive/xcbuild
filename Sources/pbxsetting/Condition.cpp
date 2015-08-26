// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Condition.h>

using pbxsetting::Condition;
using libutil::Wildcard;

Condition::
Condition(std::unordered_map<std::string, std::string> const &values) :
    _values(values)
{
}

Condition::
~Condition()
{
}

bool Condition::
match(Condition const &condition) const
{
    auto OV = condition._values;
    for (auto const &TE : _values) {
        auto OE = OV.find(TE.first);
        if (OE == OV.end()) {
            return false;
        }

        if (!Wildcard::Match(TE.second, OE->second)) {
            return false;
        }
    }

    return true;
}

Condition const &Condition::
Empty(void)
{
    static Condition *condition = new Condition({ });
    return *condition;
}
