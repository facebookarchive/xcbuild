/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <gtest/gtest.h>
#include <acdriver/Options.h>

using acdriver::Options;

TEST(Options, InvalidDocumentedOptions)
{
    Options unknown;
    auto result1 = libutil::Options::Parse<Options>(&unknown, { "--write", "path" });
    EXPECT_FALSE(result1.first);
}

