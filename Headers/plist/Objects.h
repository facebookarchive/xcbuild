/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Objects_h
#define __plist_Objects_h

#include <plist/Object.h>
#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/Data.h>
#include <plist/Date.h>
#include <plist/Dictionary.h>
#include <plist/Integer.h>
#include <plist/Null.h>
#include <plist/Real.h>
#include <plist/String.h>

namespace plist {

static inline char const *
GetObjectTypeName(int type)
{
    if (type == plist::String::Type())
        return "string";
    if (type == plist::Boolean::Type())
        return "boolean";
    if (type == plist::Array::Type())
        return "array";
    if (type == plist::Dictionary::Type())
        return "dictionary";
    if (type == plist::Integer::Type())
        return "integer";
    if (type == plist::Real::Type())
        return "real";
    if (type == plist::Data::Type())
        return "data";
    if (type == plist::Date::Type())
        return "date";
    if (type == plist::Null::Type())
        return "null";
    return "unknown";
}

}

#endif  // !__plist_Objects_h
