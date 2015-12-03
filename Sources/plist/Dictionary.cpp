/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Dictionary.h>

using plist::Object;
using plist::Dictionary;

std::unique_ptr<Dictionary> Dictionary::
New()
{
    return std::unique_ptr<Dictionary>(new Dictionary());
}

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

std::unique_ptr<Dictionary> Dictionary::
Coerce(Object const *obj)
{
    Dictionary *result = nullptr;

    if (obj->type() == Type()) {
        result = CastTo<Dictionary>(obj->copy());
    }

    return std::unique_ptr<Dictionary>(result);
}
