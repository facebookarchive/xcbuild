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
    auto info = MakefileDependencyInfo::Create("");
    ASSERT_NE(info, nullptr);
    EXPECT_EQ(info->outputInputs().size(), 0);
    EXPECT_EQ(info->dependencyInfo().inputs().size(), 0);
    EXPECT_EQ(info->dependencyInfo().outputs().size(), 0);
}

TEST(MakefileDependencyInfo, Invalid)
{
    EXPECT_EQ(MakefileDependencyInfo::Create(":"), nullptr);
    EXPECT_EQ(MakefileDependencyInfo::Create(": input"), nullptr);
    EXPECT_EQ(MakefileDependencyInfo::Create("out%put:"), nullptr);
    EXPECT_EQ(MakefileDependencyInfo::Create("output: :"), nullptr);
    EXPECT_EQ(MakefileDependencyInfo::Create("output: input%"), nullptr);
    EXPECT_EQ(MakefileDependencyInfo::Create("output: input\ninput"), nullptr);
}

TEST(MakefileDependencyInfo, NoInputs)
{
    auto info1 = MakefileDependencyInfo::Create("output:");
    ASSERT_NE(info1, nullptr);
    EXPECT_TRUE(info1->dependencyInfo().inputs().empty());
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));

    auto info2 = MakefileDependencyInfo::Create("output1: \n output2: \n");
    ASSERT_NE(info2, nullptr);
    EXPECT_TRUE(info2->dependencyInfo().inputs().empty());
    EXPECT_EQ(info2->dependencyInfo().outputs(), std::vector<std::string>({ "output1", "output2" }));
}

TEST(MakefileDependencyInfo, Inputs)
{
    auto info1 = MakefileDependencyInfo::Create("output: input1 input2");
    ASSERT_NE(info1, nullptr);
    EXPECT_EQ(info1->dependencyInfo().inputs(), std::vector<std::string>({ "input1", "input2" }));
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));

    auto info2 = MakefileDependencyInfo::Create("output1: input1\noutput2: input2");
    ASSERT_NE(info2, nullptr);
    EXPECT_EQ(info2->dependencyInfo().inputs(), std::vector<std::string>({ "input1", "input2" }));
    EXPECT_EQ(info2->dependencyInfo().outputs(), std::vector<std::string>({ "output1", "output2" }));

    auto info3 = MakefileDependencyInfo::Create("output: input1\\\n    input2  input3");
    ASSERT_NE(info3, nullptr);
    EXPECT_EQ(info3->dependencyInfo().inputs(), std::vector<std::string>({ "input1", "input2", "input3" }));
    EXPECT_EQ(info3->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));
}

TEST(MakefileDependencyInfo, Escapes)
{
    auto info1 = MakefileDependencyInfo::Create("out\\%put: in\\%put");
    ASSERT_NE(info1, nullptr);
    EXPECT_EQ(info1->dependencyInfo().inputs(), std::vector<std::string>({ "in%put" }));
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "out%put" }));

    auto info2 = MakefileDependencyInfo::Create("out\\#put: in\\#put");
    ASSERT_NE(info2, nullptr);
    EXPECT_EQ(info2->dependencyInfo().inputs(), std::vector<std::string>({ "in#put" }));
    EXPECT_EQ(info2->dependencyInfo().outputs(), std::vector<std::string>({ "out#put" }));

    auto info3 = MakefileDependencyInfo::Create("out\\put: in\\put");
    ASSERT_NE(info3, nullptr);
    EXPECT_EQ(info3->dependencyInfo().inputs(), std::vector<std::string>({ "in\\put" }));
    EXPECT_EQ(info3->dependencyInfo().outputs(), std::vector<std::string>({ "out\\put" }));

    auto info4 = MakefileDependencyInfo::Create("out\\ put: in\\ put");
    ASSERT_NE(info4, nullptr);
    EXPECT_EQ(info4->dependencyInfo().inputs(), std::vector<std::string>({ "in put" }));
    EXPECT_EQ(info4->dependencyInfo().outputs(), std::vector<std::string>({ "out put" }));
}

TEST(MakefileDependencyInfo, Comments)
{
    auto info1 = MakefileDependencyInfo::Create("output: # input1 \\\n input2");
    ASSERT_NE(info1, nullptr);
    EXPECT_TRUE(info1->dependencyInfo().inputs().empty());
    EXPECT_EQ(info1->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));

    auto info2 = MakefileDependencyInfo::Create("# output: input1 input2");
    ASSERT_NE(info2, nullptr);
    EXPECT_TRUE(info2->dependencyInfo().outputs().empty());
    EXPECT_TRUE(info2->dependencyInfo().inputs().empty());

    auto info3 = MakefileDependencyInfo::Create("# output1: input1 input2\noutput2: input3 input4");
    ASSERT_NE(info3, nullptr);
    EXPECT_EQ(info3->dependencyInfo().inputs(), std::vector<std::string>({ "input3", "input4" }));
    EXPECT_EQ(info3->dependencyInfo().outputs(), std::vector<std::string>({ "output2" }));

    auto info4 = MakefileDependencyInfo::Create("# comment1\n# comment2\n# comment3\noutput:");
    ASSERT_NE(info4, nullptr);
    EXPECT_TRUE(info4->dependencyInfo().inputs().empty());
    EXPECT_EQ(info4->dependencyInfo().outputs(), std::vector<std::string>({ "output" }));
}

