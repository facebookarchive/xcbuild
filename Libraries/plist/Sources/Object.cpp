/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Object.h>

using plist::Object;

char const *Object::
GetTypeName(enum Object::Type type)
{
    switch (type) {
        case Type::None:       return "object";
        case Type::Integer:    return "integer";
        case Type::Real:       return "real";
        case Type::String:     return "string";
        case Type::Boolean:    return "boolean";
        case Type::Null:       return "null";
        case Type::Array:      return "array";
        case Type::Dictionary: return "dictionary";
        case Type::Data:       return "data";
        case Type::Date:       return "date";
        default:              break;
    }
    return "unknown";
}

std::unique_ptr<Object> Object::
Coerce(Object const *obj)
{
    return obj->copy();
}
