/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/SimpleXML.h>

using plist::Format::Type;
using plist::Format::Encoding;
using plist::Format::Base;
using plist::Format::SimpleXML;
using plist::Object;

SimpleXML::
SimpleXML(Encoding encoding) :
    _encoding(encoding)
{
}

template<>
Type Base<SimpleXML>::
Type()
{
    return Type::SimpleXML;
}

template<>
std::unique_ptr<SimpleXML> Base<SimpleXML>::
Identify(std::vector<uint8_t> const &contents)
{
    return nullptr;
}

template<>
std::pair<Object *, std::string> Base<SimpleXML>::
Deserialize(std::vector<uint8_t> const &contents, SimpleXML const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Base<SimpleXML>::
Serialize(Object *object, SimpleXML const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

SimpleXML SimpleXML::
Create(Encoding encoding)
{
    return SimpleXML(encoding);
}
