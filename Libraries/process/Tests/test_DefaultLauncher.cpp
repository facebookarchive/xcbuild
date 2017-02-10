/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <process/DefaultLauncher.h>
#include <process/LaunchResult.h>
#include <process/MemoryContext.h>
#include <libutil/DefaultFilesystem.h>

#include <unistd.h>

using process::DefaultLauncher;
using process::MemoryContext;
using libutil::DefaultFilesystem;

TEST(DefaultLauncher, CapturesStdout)
{
    DefaultLauncher launcher;
    auto filesystem = DefaultFilesystem::GetDefaultUNSAFE();

    std::unordered_map<std::string, std::string> environment;
    auto echo = MemoryContext(
        "/bin/echo",
        "/",
        { "foo" },
        std::unordered_map<std::string, std::string>(),
        ::getuid(),
        ::getgid(),
        "user",
        "group");
    auto result = launcher.launch(filesystem, &echo);
    EXPECT_TRUE(result);
    EXPECT_EQ(0, result->exitCode());
    EXPECT_EQ("foo\n", (*result).standardOutput());
}
