/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Integer.h>
#include <plist/String.h>

using plist::Object;
using plist::Integer;
using plist::String;

Object *Integer::
copy() const
{
    return new Integer(value());
}

std::unique_ptr<Integer> Integer::
Coerce(Object const *obj)
{
    Integer *result = nullptr;

    if (obj->type() == Type()) {
        result = CastTo<Integer>(obj->copy());
    } else if (String const *string = CastTo<String>(obj)) {
        char *end = NULL;
        long long integer = std::strtoll(string->value().c_str(), &end, 0);

        if (end != string->value().c_str()) {
            result = Integer::New(integer);
        }
    }

    return std::unique_ptr<Integer>(result);
}
