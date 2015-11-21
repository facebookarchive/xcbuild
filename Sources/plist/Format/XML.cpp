/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/XML.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Base;
using plist::Format::XML;
using plist::Object;

XML::
XML(Encoding encoding) :
    _encoding(encoding)
{
}

template<>
Type Base<XML>::
Type()
{
    return Type::XML;
}

template<>
std::unique_ptr<XML> Base<XML>::
Identify(std::vector<uint8_t> const &contents)
{
    return nullptr;
}

template<>
std::pair<Object *, std::string> Base<XML>::
Deserialize(std::vector<uint8_t> const &contents, XML const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Base<XML>::
Serialize(Object *object, XML const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

XML XML::
Create(Encoding encoding)
{
    return XML(encoding);
}
