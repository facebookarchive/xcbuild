/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Array.h>

using plist::Object;
using plist::Array;

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

