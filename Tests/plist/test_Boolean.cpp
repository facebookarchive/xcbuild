/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <plist/Boolean.h>

using plist::Boolean;

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

