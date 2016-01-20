/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <plist/Format/JSON.h>
#include <plist/Objects.h>

using plist::Format::JSON;
using plist::Format::Encoding;
using plist::String;
using plist::Boolean;
using plist::Integer;
using plist::Real;
using plist::Dictionary;
using plist::Array;

static std::vector<uint8_t>
Contents(std::string const &string)
{
    return std::vector<uint8_t>(string.begin(), string.end());
}

TEST(JSON, Serialize)
{
    auto dictionary = Dictionary::New();
    dictionary->set("boolean", Boolean::New(true));
    dictionary->set("integer", Integer::New(42));
    dictionary->set("real", Real::New(3.14));

    auto serialize = JSON::Serialize(dictionary.get(), JSON::Create());
    ASSERT_NE(serialize.first, nullptr);
    EXPECT_EQ(*serialize.first, Contents("{\n\t\"boolean\": true,\n\t\"integer\": 42,\n\t\"real\": 3.14\n}"));
}

TEST(JSON, SerializeCollections)
{
    auto dict = Dictionary::New();
    dict->set("one", String::New("1"));
    dict->set("two", Integer::New(1));

    auto array = Array::New();
    array->append(String::New("test"));
    array->append(Integer::New(99));

    auto dictionary = Dictionary::New();
    dictionary->set("dict", std::move(dict));
    dictionary->set("array", std::move(array));

    auto serialize = JSON::Serialize(dictionary.get(), JSON::Create());
    ASSERT_NE(serialize.first, nullptr);
    EXPECT_EQ(*serialize.first, Contents("{\n\t\"dict\": {\n\t\t\"one\": \"1\",\n\t\t\"two\": 1\n\t},\n\t\"array\": [\n\t\t\"test\",\n\t\t99\n\t]\n}"));
}
