/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Null.h>

using plist::Object;
using plist::Null;

Null const Null::kNull;

std::unique_ptr<Null> Null::
New()
{
    return std::unique_ptr<Null>(const_cast<Null *>(&kNull));
}

Object *Null::
copy() const
{
    return const_cast <Null *> (this);
}

std::unique_ptr<Null> Null::
Coerce(Object const *obj)
{
    Null *result = nullptr;

    if (obj->type() == Type()) {
        result = CastTo<Null>(obj->copy());
    }

    return std::unique_ptr<Null>(result);
}
