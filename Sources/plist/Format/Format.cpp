/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/Format.h>

using plist::Object;
using plist::Format::Format;
using plist::Format::Info;
using plist::Format::Type;

template<typename T>
static std::unique_ptr<Info>
IdentifyImpl(std::vector<uint8_t> const &contents)
{
    std::unique_ptr<T> result = T::Identify(contents);
    if (result != nullptr) {
        return std::make_unique<Info>(Info::Create<T>(*result));
    }

    return nullptr;
}

std::unique_ptr<Info> Format::
Identify(std::vector<uint8_t> const &contents)
{
#define FORMAT(T) \
    { \
        std::unique_ptr<Info> result = IdentifyImpl<T>(contents); \
        if (result != nullptr) { \
            return result; \
        } \
    }

    FORMAT(Binary);
    FORMAT(XML);
    FORMAT(ASCII);
    FORMAT(Strings);
    FORMAT(SimpleXML);

#undef FORMAT

    return nullptr;
}

template<typename T>
static std::pair<Object *, std::string>
DeserializeImpl(std::vector<uint8_t> const &contents, Info const &info)
{
    return T::Deserialize(contents, *info.format<T>());
}

std::pair<Object *, std::string> Format::
Deserialize(std::vector<uint8_t> const &contents, Info const &info)
{
    switch (info.type()) {
        case Type::Binary:
            return DeserializeImpl<Binary>(contents, info);
        case Type::XML:
            return DeserializeImpl<XML>(contents, info);
        case Type::ASCII:
            return DeserializeImpl<ASCII>(contents, info);
        case Type::Strings:
            return DeserializeImpl<Strings>(contents, info);
        case Type::SimpleXML:
            return DeserializeImpl<SimpleXML>(contents, info);
    }
}

template<typename T>
static std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string>
SerializeImpl(Object *object, Info const &info)
{
    return T::Serialize(object, *info.format<T>());
}

std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format::
Serialize(Object *object, Info const &info)
{
    if (object == nullptr) {
        return std::make_pair(std::make_unique<std::vector<uint8_t>>(std::vector<uint8_t>()), "invalid object to serialize");
    }

    switch (info.type()) {
        case Type::Binary:
            return SerializeImpl<Binary>(object, info);
        case Type::XML:
            return SerializeImpl<XML>(object, info);
        case Type::ASCII:
            return SerializeImpl<ASCII>(object, info);
        case Type::Strings:
            return SerializeImpl<Strings>(object, info);
        case Type::SimpleXML:
            return SerializeImpl<SimpleXML>(object, info);
    }
}
