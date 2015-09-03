
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

