/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Date.h>

using plist::Object;
using plist::Date;

std::unique_ptr<Date> Date::
New(struct tm const &value)
{
    return std::unique_ptr<Date>(new Date(value));
}

std::unique_ptr<Date> Date::
New(std::string const &value)
{
    return std::unique_ptr<Date>(new Date(value));
}

std::unique_ptr<Date> Date::
New(uint64_t value)
{
    return std::unique_ptr<Date>(new Date(value));
}

Object *Date::
copy() const
{
    return new Date(_value);
}

std::unique_ptr<Date> Date::
Coerce(Object const *obj)
{
    Date *result = nullptr;

    if (obj->type() == Type()) {
        result = CastTo<Date>(obj->copy());
    }

    return std::unique_ptr<Date>(result);
}
