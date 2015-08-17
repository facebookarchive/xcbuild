// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Condition.h>

using pbxsetting::Condition;

Condition::
Condition(std::unordered_map<std::string, std::string> const &values) :
    _values(values)
{
}

Condition::
~Condition()
{
}

static bool
WildcardMatch(std::string const &first, std::string const &second)
{
    std::string::const_iterator sit = second.begin();
    for (std::string::const_iterator fit = first.begin(); fit != first.end(); ++fit) {
        if (*fit == '*') {
            if (++fit == first.end() || sit == second.end()) {
                return true;
            }

            while (*sit != *fit) {
                if (++sit == second.end()) {
                    return false;
                }
            }
        } else if (*fit != *sit++) {
            return false;
        }
    }

    return true;
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

        if (!WildcardMatch(TE.second, OE->second)) {
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
