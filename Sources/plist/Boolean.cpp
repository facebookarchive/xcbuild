/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Boolean.h>
#include <plist/String.h>
#include <plist/Integer.h>
#include <plist/Real.h>

using plist::Object;
using plist::Boolean;
using plist::Integer;
using plist::Real;
using plist::String;

Boolean const Boolean::kTrue(true);
Boolean const Boolean::kFalse(false);

std::unique_ptr<Boolean> Boolean::
New(bool value)
{
    return std::unique_ptr<Boolean>(const_cast<Boolean *>(value ? &kTrue : &kFalse));
}

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

std::unique_ptr<Boolean> Boolean::
Coerce(Object const *obj)
{
    if (obj->type() == Type()) {
        return std::unique_ptr<Boolean>(CastTo<Boolean>(obj->copy()));
    } else if (String const *string = CastTo<String>(obj)) {
        bool value = (strcasecmp(string->value().c_str(), "yes") == 0 || strcasecmp(string->value().c_str(), "true") == 0);
        return Boolean::New(value);
    } else if (Integer const *integer = CastTo<Integer>(obj)) {
        return Boolean::New(integer->value() != 0);
    } else if (Real const *real = CastTo<Real>(obj)) {
        return Boolean::New(real->value() != 0.0);
    }

    return nullptr;
}
