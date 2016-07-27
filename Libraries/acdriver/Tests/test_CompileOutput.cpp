/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <acdriver/Compile/Output.h>
#include <acdriver/Result.h>
#include <libutil/MemoryFilesystem.h>

namespace Compile = acdriver::Compile;
using acdriver::Result;
using libutil::MemoryFilesystem;

static std::vector<uint8_t>
Contents(std::string const &string)
{
    return std::vector<uint8_t>(string.begin(), string.end());
}

TEST(CompileOutput, Empty)
{
    MemoryFilesystem filesystem = MemoryFilesystem({ });
    Compile::Output output = Compile::Output("/", Compile::Output::Format::Compiled);

    Result result;
    EXPECT_TRUE(output.write(&filesystem, ext::nullopt, ext::nullopt, &result));
    EXPECT_TRUE(result.success());
}

TEST(CompileOutput, Copy)
{
    MemoryFilesystem filesystem = MemoryFilesystem({
        MemoryFilesystem::Entry::File("source1", Contents("one")),
        MemoryFilesystem::Entry::File("source2", Contents("two")),
    });

    /* Copy files from input to output. */
    Compile::Output output = Compile::Output("/", Compile::Output::Format::Compiled);
    output.copies().push_back({ "/source1", "/dest1" });
    output.copies().push_back({ "/source2", "/dest2" });

    Result result;
    EXPECT_TRUE(output.write(&filesystem, ext::nullopt, ext::nullopt, &result));
    EXPECT_TRUE(result.success());

    /* Contents should have been copied. */
    std::vector<uint8_t> contents;
    ASSERT_TRUE(filesystem.read(&contents, "/dest1"));
    EXPECT_EQ(contents, Contents("one"));
    ASSERT_TRUE(filesystem.read(&contents, "/dest2"));
    EXPECT_EQ(contents, Contents("two"));
}
