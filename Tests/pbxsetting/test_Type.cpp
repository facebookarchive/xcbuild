/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <pbxsetting/Type.h>

using pbxsetting::Type;

TEST(Type, Boolean)
{
    EXPECT_EQ(Type::ParseBoolean("NO"), false);
    EXPECT_EQ(Type::ParseBoolean(""), false);
    EXPECT_EQ(Type::ParseBoolean("YES"), true);
    EXPECT_EQ(Type::ParseBoolean("yes"), true);
    EXPECT_EQ(Type::ParseBoolean("Y"), true);
}

TEST(Type, List)
{
    EXPECT_EQ(Type::ParseList("hello"), std::vector<std::string>({ "hello" }));
    EXPECT_EQ(Type::ParseList("hello world"), std::vector<std::string>({ "hello", "world" }));
    EXPECT_EQ(Type::ParseList("  hello  world  "), std::vector<std::string>({ "hello", "world" }));
    EXPECT_EQ(Type::ParseList("hello 'world'"), std::vector<std::string>({ "hello", "world" }));
    EXPECT_EQ(Type::ParseList("'hello world'"), std::vector<std::string>({ "hello world" }));
    EXPECT_EQ(Type::ParseList("\"hello world\""), std::vector<std::string>({ "hello world" }));
    EXPECT_EQ(Type::ParseList("\"hello\" \"world\""), std::vector<std::string>({ "hello", "world" }));
    EXPECT_EQ(Type::ParseList("'hello wo'rld"), std::vector<std::string>({ "hello world" }));
    EXPECT_EQ(Type::ParseList("hell'o wo'rld"), std::vector<std::string>({ "hello world" }));
    EXPECT_EQ(Type::ParseList("'' '' 'test'"), std::vector<std::string>({ "test" }));
}
