/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/Strings.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Base;
using plist::Format::Strings;
using plist::Object;

Strings::
Strings(Encoding encoding) :
    _encoding(encoding)
{
}

template<>
Type Base<Strings>::
Type()
{
    return Type::Strings;
}

template<>
std::unique_ptr<Strings> Base<Strings>::
Identify(std::vector<uint8_t> const &contents)
{
    return nullptr;
}

template<>
std::pair<Object *, std::string> Base<Strings>::
Deserialize(std::vector<uint8_t> const &contents, Strings const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Base<Strings>::
Serialize(Object *object, Strings const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

Strings Strings::
Create(Encoding encoding)
{
    return Strings(encoding);
}
