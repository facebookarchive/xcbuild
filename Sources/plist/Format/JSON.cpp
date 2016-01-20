/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/JSON.h>
#include <plist/Format/JSONWriter.h>

using plist::Format::Encoding;
using plist::Format::Format;
using plist::Format::JSON;
using plist::Format::JSONWriter;
using plist::Object;

JSON::
JSON()
{
}

template<>
std::unique_ptr<JSON> Format<JSON>::
Identify(std::vector<uint8_t> const &contents)
{
    /* JSON is not a standard format. */
    return nullptr;
}

template<>
std::pair<std::unique_ptr<Object>, std::string> Format<JSON>::
Deserialize(std::vector<uint8_t> const &contents, JSON const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format<JSON>::
Serialize(Object const *object, JSON const &format)
{
    if (object == nullptr) {
        return std::make_pair(nullptr, "object was null");
    }

    JSONWriter writer = JSONWriter(object);
    if (!writer.write()) {
        return std::make_pair(nullptr, "serialization failed");
    }

    return std::make_pair(std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>(writer.contents())), std::string());
}

JSON JSON::
Create()
{
    return JSON();
}
