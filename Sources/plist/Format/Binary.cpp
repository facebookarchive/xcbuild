/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/Binary.h>

using plist::Format::Type;
using plist::Format::Base;
using plist::Format::Binary;
using plist::Object;

Binary::
Binary()
{
}

template<>
Type Base<Binary>::
Type()
{
    return Type::Binary;
}

template<>
std::unique_ptr<Binary> Base<Binary>::
Identify(std::vector<uint8_t> const &contents)
{
    return nullptr;
}

template<>
std::pair<Object *, std::string> Base<Binary>::
Deserialize(std::vector<uint8_t> const &contents, Binary const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Base<Binary>::
Serialize(Object *object, Binary const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

Binary Binary::
Create()
{
    return Binary();
}
