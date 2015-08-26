
#include <gtest/gtest.h>
#include <libutil/Wildcard.h>

using libutil::Wildcard;

TEST(Wildcard, Basic)
{
    EXPECT_TRUE(Wildcard::Match("", ""));
    EXPECT_TRUE(Wildcard::Match("a", "a"));
    EXPECT_TRUE(Wildcard::Match("abcd", "abcd"));
    EXPECT_FALSE(Wildcard::Match("abc", "abcd"));
    EXPECT_FALSE(Wildcard::Match("abcd", "bcd"));
}

TEST(Wildcard, Star)
{
    EXPECT_TRUE(Wildcard::Match("*", ""));
    EXPECT_TRUE(Wildcard::Match("*", "a"));
    EXPECT_TRUE(Wildcard::Match("a*", "a"));
    EXPECT_TRUE(Wildcard::Match("*a", "a"));
    EXPECT_TRUE(Wildcard::Match("*a*", "a"));
    EXPECT_TRUE(Wildcard::Match("*", "abcd"));
    EXPECT_TRUE(Wildcard::Match("a*de", "abcde"));
    EXPECT_FALSE(Wildcard::Match("a*d", "abcde"));
    EXPECT_FALSE(Wildcard::Match("a*dce", "abcde"));
    EXPECT_FALSE(Wildcard::Match("*a", "b"));
}

TEST(Wildcard, One)
{
    EXPECT_TRUE(Wildcard::Match("[", "["));
    EXPECT_TRUE(Wildcard::Match("[a]", "a"));
    EXPECT_TRUE(Wildcard::Match("[aA]", "A"));
    EXPECT_TRUE(Wildcard::Match("b[aA]d", "bAd"));
    EXPECT_TRUE(Wildcard::Match("b[aA]d", "bad"));
    EXPECT_TRUE(Wildcard::Match("b[aei][dn]", "ban"));
    EXPECT_TRUE(Wildcard::Match("b[aei][dn]", "bid"));

    EXPECT_FALSE(Wildcard::Match("[aA]", "aA"));
    EXPECT_FALSE(Wildcard::Match("[aA]", "b"));
}

