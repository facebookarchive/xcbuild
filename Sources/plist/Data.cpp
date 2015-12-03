/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Data.h>

using plist::Object;
using plist::Data;

std::unique_ptr<Data> Data::
New(std::vector<uint8_t> const &value)
{
    return std::unique_ptr<Data>(new Data(value));
}

std::unique_ptr<Data> Data::
New(std::vector<uint8_t> &&value)
{
    return std::unique_ptr<Data>(new Data(value));
}

std::unique_ptr<Data> Data::
New(std::string const &value)
{
    return std::unique_ptr<Data>(new Data(value));
}

std::unique_ptr<Data> Data::
New(void const *bytes, size_t length)
{
    return std::unique_ptr<Data>(new Data(bytes, length));
}

Object *Data::
copy() const
{
    return new Data(_value);
}

std::unique_ptr<Data> Data::
Coerce(Object const *obj)
{
    Data *result = nullptr;

    if (obj->type() == Type()) {
        result = CastTo<Data>(obj->copy());
    }

    return std::unique_ptr<Data>(result);
}
