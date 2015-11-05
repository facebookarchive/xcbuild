/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <libutil/FSUtil.h>

using libutil::FSUtil;

TEST(FSUtil, Relative)
{
    EXPECT_EQ("", FSUtil::GetRelativePath("/", "/"));
    EXPECT_EQ("", FSUtil::GetRelativePath("/a", "/a"));
    EXPECT_EQ("", FSUtil::GetRelativePath("/a/b", "/a/b"));
    EXPECT_EQ("a", FSUtil::GetRelativePath("/a", "/"));
    EXPECT_EQ("a/b", FSUtil::GetRelativePath("/a/b", "/"));
    EXPECT_EQ("b", FSUtil::GetRelativePath("/a/b", "/a"));
    EXPECT_EQ("../a", FSUtil::GetRelativePath("/a", "/b"));
    EXPECT_EQ("../a/b", FSUtil::GetRelativePath("/a/b", "/b"));
    EXPECT_EQ("../b", FSUtil::GetRelativePath("/a/b", "/a/c"));
    EXPECT_EQ("../bbb", FSUtil::GetRelativePath("/aaa/bbb", "/aaa/ccc"));
}

