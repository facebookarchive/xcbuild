/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Boolean.h>
#include <plist/String.h>

using plist::Object;
using plist::Boolean;
using plist::String;

Boolean const Boolean::kTrue(true);
Boolean const Boolean::kFalse(false);

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