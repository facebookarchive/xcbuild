/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <plist/Objects.h>

using plist::Boolean;
using plist::String;
using plist::Dictionary;

TEST(Boolean, New)
{
    std::unique_ptr<Boolean> t = std::unique_ptr<Boolean>(Boolean::New(true));
    EXPECT_TRUE(t->value());

    std::unique_ptr<Boolean> f = std::unique_ptr<Boolean>(Boolean::New(false));
    EXPECT_FALSE(f->value());
}

TEST(Boolean, Identity)
{
    std::unique_ptr<Boolean> t1 = std::unique_ptr<Boolean>(Boolean::New(true));
    std::unique_ptr<Boolean> t2 = std::unique_ptr<Boolean>(Boolean::New(true));
    EXPECT_EQ(t1, t2);

    std::unique_ptr<Boolean> f1 = std::unique_ptr<Boolean>(Boolean::New(false));
    std::unique_ptr<Boolean> f2 = std::unique_ptr<Boolean>(Boolean::New(false));
    EXPECT_EQ(f1, f2);
}

TEST(Boolean, Coerce)
{
    auto s1 = std::unique_ptr<String>(String::New("yes"));
    auto b1 = std::unique_ptr<Boolean>(Boolean::Coerce(s1.get()));
    EXPECT_TRUE(b1->value());

    auto s2 = std::unique_ptr<String>(String::New("YES"));
    auto b2 = std::unique_ptr<Boolean>(Boolean::Coerce(s2.get()));
    EXPECT_TRUE(b2->value());

    auto s3 = std::unique_ptr<String>(String::New("Yes"));
    auto b3 = std::unique_ptr<Boolean>(Boolean::Coerce(s3.get()));
    EXPECT_TRUE(b3->value());

    auto s4 = std::unique_ptr<String>(String::New("trUE"));
    auto b4 = std::unique_ptr<Boolean>(Boolean::Coerce(s4.get()));
    EXPECT_TRUE(b4->value());

    auto s5 = std::unique_ptr<String>(String::New("NO"));
    auto b5 = std::unique_ptr<Boolean>(Boolean::Coerce(s5.get()));
    EXPECT_FALSE(b5->value());

    auto s6 = std::unique_ptr<String>(String::New("false"));
    auto b6 = std::unique_ptr<Boolean>(Boolean::Coerce(s6.get()));
    EXPECT_FALSE(b6->value());

    auto s7 = std::unique_ptr<String>(String::New(""));
    auto b7 = std::unique_ptr<Boolean>(Boolean::Coerce(s7.get()));
    EXPECT_FALSE(b7->value());

    auto a8 = std::unique_ptr<Dictionary>(Dictionary::New());
    auto b8 = std::unique_ptr<Boolean>(Boolean::Coerce(a8.get()));
    EXPECT_EQ(b8, nullptr);
}
