/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Keys.h>
#include <plist/Object.h>
#include <plist/Boolean.h>

using plist::Object;
using plist::Boolean;
using plist::Dictionary;
using plist::WarnUnhandledKeyMap;

void plist::
WarnUnhandledKeyMap(Dictionary const *dict, char const *name, plist::KeyTypeMap const &keys)
{
    if (dict == nullptr)
        return;

    for (size_t n = 0; n < dict->count(); n++) {
        auto I = keys.find(dict->key(n));
        if (I == keys.end()) {
            fprintf(stderr, "warning: %s%s%s"
                    "key '%s' of type '%s' is not handled\n",
                    name != nullptr ? "parsing " : "",
                    name != nullptr ? name : "",
                    name != nullptr ? " " : "",
                    dict->key(n).c_str(),
                    dict->value(n)->typeName());
            continue;
        }

        //
        // If the type used is Object, any type will do!
        //
        if (I->second == Object::Type())
            continue;

        //
        // Handle special boolean case for broken plists that
        // use strings in place of booleans.
        //
        if (I->second == Boolean::Type() &&
                CastTo <Boolean> (dict->value(n)) != nullptr)
            continue;

        if (I->second != dict->value(n)->type()) {
            fprintf(stderr, "warning: %s%s%s"
                    "expecting key '%s' to be of type "
                    "'%s' but it is of type '%s'\n",
                    name != nullptr ? "parsing " : "",
                    name != nullptr ? name : "",
                    name != nullptr ? " " : "",
                    I->first.c_str(),
                    Object::GetTypeName(I->second),
                    dict->value(n)->typeName());
        }
    }
}
