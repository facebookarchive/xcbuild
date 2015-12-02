/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Real.h>
#include <plist/String.h>

using plist::Object;
using plist::Real;
using plist::String;

Object *Real::
copy() const
{
    return new Real(value());
}

std::unique_ptr<Real> Real::
Coerce(Object const *obj)
{
    Real *result = nullptr;

    if (obj->type() == Type()) {
        result = CastTo<Real>(obj->copy());
    } else if (String const *string = CastTo<String>(obj)) {
        char *end = NULL;
        double real = std::strtod(string->value().c_str(), &end);

        if (end != string->value().c_str()) {
            result = Real::New(real);
        }
    }

    return std::unique_ptr<Real>(result);
}
