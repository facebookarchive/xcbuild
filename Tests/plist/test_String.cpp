/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <plist/Objects.h>

using plist::String;
using plist::Boolean;
using plist::Real;
using plist::Integer;
using plist::Array;

TEST(String, New)
{
    auto s1 = std::unique_ptr<String>(String::New("test"));
    EXPECT_EQ(s1->value(), "test");

    auto s2 = std::unique_ptr<String>(String::New(""));
    EXPECT_EQ(s2->value(), "");
}

TEST(String, Coerce)
{
    auto b1 = std::unique_ptr<Boolean>(Boolean::New(false));
    auto s1 = std::unique_ptr<String>(String::Coerce(b1.get()));
    EXPECT_EQ(s1->value(), "NO");

    auto b2 = std::unique_ptr<Boolean>(Boolean::New(true));
    auto s2 = std::unique_ptr<String>(String::Coerce(b2.get()));
    EXPECT_EQ(s2->value(), "YES");

    auto r3 = std::unique_ptr<Real>(Real::New(1.0));
    auto s3 = std::unique_ptr<String>(String::Coerce(r3.get()));
    EXPECT_EQ(s3->value(), "1");

    auto r4 = std::unique_ptr<Real>(Real::New(3.14));
    auto s4 = std::unique_ptr<String>(String::Coerce(r4.get()));
    EXPECT_EQ(s4->value(), "3.14");

    auto i5 = std::unique_ptr<Integer>(Integer::New(1));
    auto s5 = std::unique_ptr<String>(String::Coerce(i5.get()));
    EXPECT_EQ(s5->value(), "1");

    auto i6 = std::unique_ptr<Integer>(Integer::New(65536));
    auto s6 = std::unique_ptr<String>(String::Coerce(i6.get()));
    EXPECT_EQ(s6->value(), "65536");

    auto a7 = std::unique_ptr<Array>(Array::New());
    auto s7 = std::unique_ptr<String>(String::Coerce(a7.get()));
    EXPECT_EQ(s7, nullptr);
}
