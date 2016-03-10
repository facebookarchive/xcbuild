/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <dependency/MakefileDependencyInfo.h>

using dependency::MakefileDependencyInfo;
using dependency::DependencyInfo;

TEST(MakefileDependencyInfo, Empty)
{
    auto info = MakefileDependencyInfo::Deserialize("");
    ASSERT_TRUE(info);
    EXPECT_EQ(info->outputInputs().size(), 0);
    EXPECT_EQ(info->dependencyInfo().inputs().size(), 0);
    EXPECT_EQ(info->dependencyInfo().outputs().size(), 0);
}

TEST(MakefileDependencyInfo, Invalid)
{
    EXPECT_FALSE(MakefileDependencyInfo::Deserialize(":"));
    EXPECT_FALSE(MakefileDependencyInfo::Deserialize(": input"));
    EXPECT_FALSE(MakefileDependencyInfo::Deserialize("out%put:"));
    EXPECT_FALSE(MakefileDependencyInfo::Deserialize("output: :"));
    EXPECT_FALSE(MakefileDependencyInfo::Deserialize("output: input%"));
    EXPECT_FALSE(MakefileDependencyInfo::Deserialize("output: input\ninput"));
}

TEST(MakefileDependencyInfo, NoInputs)
{
    auto info1 = MakefileDependencyInfo::Deserialize("output:");
    ASSERT_TRUE(info1);
    EXPECT_TRUE(info1->dependencyInfo().inputs().empty());
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));

    auto info2 = MakefileDependencyInfo::Deserialize("output1: \n output2: \n");
    ASSERT_TRUE(info2);
    EXPECT_TRUE(info2->dependencyInfo().inputs().empty());
    EXPECT_EQ(info2->dependencyInfo().outputs(), std::vector<std::string>({ "output1", "output2" }));
}

TEST(MakefileDependencyInfo, Inputs)
{
    auto info1 = MakefileDependencyInfo::Deserialize("output: input1 input2");
    ASSERT_TRUE(info1);
    EXPECT_EQ(info1->dependencyInfo().inputs(), std::vector<std::string>({ "input1", "input2" }));
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));

    auto info2 = MakefileDependencyInfo::Deserialize("output1: input1\noutput2: input2");
    ASSERT_TRUE(info2);
    EXPECT_EQ(info2->dependencyInfo().inputs(), std::vector<std::string>({ "input1", "input2" }));
    EXPECT_EQ(info2->dependencyInfo().outputs(), std::vector<std::string>({ "output1", "output2" }));

    auto info3 = MakefileDependencyInfo::Deserialize("output: input1\\\n    input2  input3");
    ASSERT_TRUE(info3);
    EXPECT_EQ(info3->dependencyInfo().inputs(), std::vector<std::string>({ "input1", "input2", "input3" }));
    EXPECT_EQ(info3->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));
}

TEST(MakefileDependencyInfo, Escapes)
{
    auto info1 = MakefileDependencyInfo::Deserialize("out\\%put: in\\%put");
    ASSERT_TRUE(info1);
    EXPECT_EQ(info1->dependencyInfo().inputs(), std::vector<std::string>({ "in%put" }));
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "out%put" }));

    auto info2 = MakefileDependencyInfo::Deserialize("out\\#put: in\\#put");
    ASSERT_TRUE(info2);
    EXPECT_EQ(info2->dependencyInfo().inputs(), std::vector<std::string>({ "in#put" }));
    EXPECT_EQ(info2->dependencyInfo().outputs(), std::vector<std::string>({ "out#put" }));

    auto info3 = MakefileDependencyInfo::Deserialize("out\\put: in\\put");
    ASSERT_TRUE(info3);
    EXPECT_EQ(info3->dependencyInfo().inputs(), std::vector<std::string>({ "in\\put" }));
    EXPECT_EQ(info3->dependencyInfo().outputs(), std::vector<std::string>({ "out\\put" }));

    auto info4 = MakefileDependencyInfo::Deserialize("out\\ put: in\\ put");
    ASSERT_TRUE(info4);
    EXPECT_EQ(info4->dependencyInfo().inputs(), std::vector<std::string>({ "in put" }));
    EXPECT_EQ(info4->dependencyInfo().outputs(), std::vector<std::string>({ "out put" }));
}

TEST(MakefileDependencyInfo, Comments)
{
    auto info1 = MakefileDependencyInfo::Deserialize("output: # input1 \\\n input2");
    ASSERT_TRUE(info1);
    EXPECT_TRUE(info1->dependencyInfo().inputs().empty());
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));

    auto info2 = MakefileDependencyInfo::Deserialize("# output: input1 input2");
    ASSERT_TRUE(info2);
    EXPECT_TRUE(info2->dependencyInfo().outputs().empty());
    EXPECT_TRUE(info2->dependencyInfo().inputs().empty());

    auto info3 = MakefileDependencyInfo::Deserialize("# output1: input1 input2\noutput2: input3 input4");
    ASSERT_TRUE(info3);
    EXPECT_EQ(info3->dependencyInfo().inputs(), std::vector<std::string>({ "input3", "input4" }));
    EXPECT_EQ(info3->dependencyInfo().outputs(), std::vector<std::string>({ "output2" }));

    auto info4 = MakefileDependencyInfo::Deserialize("# comment1\n# comment2\n# comment3\noutput:");
    ASSERT_TRUE(info4);
    EXPECT_TRUE(info4->dependencyInfo().inputs().empty());
    EXPECT_EQ(info4->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));
}

TEST(MakefileDependencyInfo, SerializeBasic)
{
    MakefileDependencyInfo info;
    info.dependencyInfo().inputs() = { "in1", "in2" };
    info.dependencyInfo().outputs() = { "out1", "out2" };

    std::string serialized = info.serialize();
    EXPECT_EQ(info.serialize(), "out1 out2: \\\n  in1 \\\n  in2");
}

TEST(MakefileDependencyInfo, SerializeMultiple)
{
    MakefileDependencyInfo info;
    info.outputInputs() = {
        { "out1", "in1a" },
        { "out1", "in1b" },
        { "out2", "in2a" },
        { "out2", "in2b" },
    };

    std::string serialized = info.serialize();
    EXPECT_EQ(info.serialize(), "out1: \\\n  in1a \\\n  in1b\n\nout2: \\\n  in2a \\\n  in2b");
}

TEST(MakefileDependencyInfo, SerializeMultipleFallback)
{
    MakefileDependencyInfo info;
    info.outputInputs() = {
        { "out1", "in1a" },
        { "out1", "in1b" },
        { "out2", "in2a" },
        { "out2", "in2b" },
    };
    info.dependencyInfo().inputs() = { "inA", "inB" };
    info.dependencyInfo().outputs() = { "outA", "outB" };

    std::string serialized = info.serialize();
    EXPECT_EQ(info.serialize(), "out1: \\\n  in1a \\\n  in1b\n\nout2: \\\n  in2a \\\n  in2b\n\noutA outB: \\\n  inA \\\n  inB");
}
