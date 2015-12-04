/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <plist/Format/ASCII.h>
#include <plist/Objects.h>

using plist::Format::ASCII;
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

TEST(ASCII, QuotedString)
{
    auto contents = Contents("\"str*ng\"\n");
    EXPECT_NE(ASCII::Identify(contents), nullptr);

    auto deserialize = ASCII::Deserialize(contents, ASCII::Create(Encoding::UTF8));
    ASSERT_NE(deserialize.first, nullptr);

    auto string = String::New("str*ng");
    EXPECT_TRUE(deserialize.first->equals(string.get()));

    auto serialize = ASCII::Serialize(deserialize.first, ASCII::Create(Encoding::UTF8));
    ASSERT_NE(serialize.first, nullptr);
    EXPECT_EQ(*serialize.first, contents);

    deserialize.first->release();
}

TEST(ASCII, UnquotedString)
{
    auto contents = Contents("string\n");
    EXPECT_NE(ASCII::Identify(contents), nullptr);

    auto deserialize = ASCII::Deserialize(contents, ASCII::Create(Encoding::UTF8));
    ASSERT_NE(deserialize.first, nullptr);

    auto string = String::New("string");
    EXPECT_TRUE(deserialize.first->equals(string.get()));

    auto serialize = ASCII::Serialize(deserialize.first, ASCII::Create(Encoding::UTF8));
    ASSERT_NE(serialize.first, nullptr);
    EXPECT_EQ(*serialize.first, contents);

    deserialize.first->release();
}

TEST(ASCII, BooleanNumberAreStrings)
{
    auto contents = Contents("{\n\tboolean = YES;\n\tinteger = 42;\n\treal = 3.14;\n}\n");
    EXPECT_NE(ASCII::Identify(contents), nullptr);

    auto deserialize = ASCII::Deserialize(contents, ASCII::Create(Encoding::UTF8));
    ASSERT_NE(deserialize.first, nullptr);

    /* Integers, reals, and booleans should always be parsed as strings. */
    auto dictionary = Dictionary::New();
    dictionary->set("boolean", String::New("YES"));
    dictionary->set("integer", String::New("42"));
    dictionary->set("real", String::New("3.14"));
    EXPECT_TRUE(deserialize.first->equals(dictionary.get()));

    deserialize.first->release();

    /* Test that real booleans, integers, and reals are serialized as strings. */
    auto typed = Dictionary::New();
    typed->set("boolean", Boolean::New(true));
    typed->set("integer", Integer::New(42));
    typed->set("real", Real::New(3.14));

    auto serialize2 = ASCII::Serialize(typed.get(), ASCII::Create(Encoding::UTF8));
    ASSERT_NE(serialize2.first, nullptr);
    EXPECT_EQ(*serialize2.first, contents);
}
