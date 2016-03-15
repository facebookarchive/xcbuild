/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <gtest/gtest.h>
#include <acdriver/Output.h>

using acdriver::Output;

TEST(Output, Text)
{
    Output output;
    output.add("output.test1", plist::String::New("test1"), "text-output1");
    output.add("output.test2", plist::String::New("test2"), "text-output2");

    ext::optional<std::vector<uint8_t>> contents = output.serialize(Output::Format::Text);
    ASSERT_TRUE(contents);

    std::string value = std::string(contents->begin(), contents->end());
    EXPECT_EQ(value, "/* output.test1 */\ntext-output1\n/* output.test2 */\ntext-output2\n");
}

