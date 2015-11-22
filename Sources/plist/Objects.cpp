/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Objects.h>
#include <plist/Keys.h>

#include <cassert>
#include <cstring>
#include <csetjmp>
#include <cerrno>

using namespace plist;

Null const Null::kNull;
Boolean const Boolean::kTrue(true);
Boolean const Boolean::kFalse(false);

//
// Object
//

char const *Object::
GetTypeName(enum Object::Type type)
{
    switch (type) {
        case kTypeInteger:    return "integer";
        case kTypeReal:       return "real";
        case kTypeString:     return "string";
        case kTypeBoolean:    return "boolean";
        case kTypeNull:       return "null";
        case kTypeArray:      return "array";
        case kTypeDictionary: return "dictionary";
        case kTypeData:       return "data";
        case kTypeDate:       return "date";
        default:              break;
    }
    return "unknown";
}

//
// Integer
//

Object *Integer::
copy() const
{
    return new Integer(value());
}

//
// Real
//

Object *Real::
copy() const
{
    return new Real(value());
}

//
// String
//

Object *String::
copy() const
{
    return new String(value());
}

//
// Boolean
//

bool Boolean::
equals(Object const *obj) const
{
    if (Object::equals(obj))
        return true;

    Boolean const *objt = CastTo <Boolean> (obj);
    return (objt != nullptr && equals(objt));
}

Object *Boolean::
copy() const
{
    return const_cast <Boolean *> (this);
}

Boolean *
Boolean::New(Object const *object)
{
    if (object->type() == Type())
        return static_cast <Boolean *> (const_cast <Object *> (object));
    if (auto s = CastTo <String> (object)) {
        if (::strcasecmp(s->value().c_str(), "yes") == 0)
            return Boolean::New(true);
        if (::strcasecmp(s->value().c_str(), "no") == 0)
            return Boolean::New(false);
    }

    return nullptr;
}

//
// Null
//

Object *Null::
copy() const
{
    return const_cast <Null *> (this);
}

//
// Array
//

Object *Array::
copy() const
{
    Array *result = new Array;
    for (size_t n = 0; n < count(); n++) {
        result->append(value(n)->copy());
    }
    return result;
}

void Array::
merge(Array const *array)
{
    if (array == nullptr || array == this)
        return;

    for (auto obj : *array) {
        append(obj->copy());
    }
}

//
// Dictionary
//

Object *Dictionary::
copy() const
{
    Dictionary *result = new Dictionary;
    for (size_t n = 0; n < count(); n++) {
        result->set(key(n), value(n)->copy());
    }
    return result;
}

void Dictionary::
merge(Dictionary const *dict, bool replace)
{
    if (dict == nullptr || dict == this)
        return;

    for (auto key : *dict) {
        if (replace || _map.find(key) == _map.end()) {
            set(key, dict->value(key)->copy());
        }
    }
}

//
// Data
//

Object *Data::
copy() const
{
    return new Data(_value);
}

//
// Date
//

Object *Date::
copy() const
{
    return new Date(_value);
}

//
// Utilities
//
using plist::WarnUnhandledKeyMap;

void plist::
WarnUnhandledKeyMap(plist::Dictionary const *dict, char const *name,
        plist::KeyTypeMap const &keys)
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
