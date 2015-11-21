/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/ASCII.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Format;
using plist::Format::ASCII;
using plist::Object;

ASCII::
ASCII(Encoding encoding) :
    _encoding(encoding)
{
}

Type ASCII::
Type()
{
    return Type::ASCII;
}

template<>
std::unique_ptr<ASCII> Format<ASCII>::
Identify(std::vector<uint8_t> const &contents)
{
    return nullptr;
}

template<>
std::pair<Object *, std::string> Format<ASCII>::
Deserialize(std::vector<uint8_t> const &contents, ASCII const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format<ASCII>::
Serialize(Object *object, ASCII const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

ASCII ASCII::
Create(Encoding encoding)
{
    return ASCII(encoding);
}
