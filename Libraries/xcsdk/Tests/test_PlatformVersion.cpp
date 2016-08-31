/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <xcsdk/SDK/PlatformVersion.h>
#include <libutil/MemoryFilesystem.h>

using xcsdk::SDK::PlatformVersion;
using libutil::MemoryFilesystem;

static std::vector<uint8_t>
Contents(std::string const &string)
{
    return std::vector<uint8_t>(string.begin(), string.end());
}

TEST(PlatformVersion, Parse)
{
    std::string platform = "Test.platform";
    auto filesystem = MemoryFilesystem({
        MemoryFilesystem::Entry::Directory(platform, {
            MemoryFilesystem::Entry::File("version.plist", Contents("{ \
                ProjectName = Name; \
                ProductBuildVersion = 1; \
                BuildVersion = 10; \
                SourceVersion = 100; \
            }")),
        }),
    });

    auto platformVersion = PlatformVersion::Open(&filesystem, "/" + platform);
    ASSERT_NE(platformVersion, nullptr);

    EXPECT_EQ(platformVersion->name(), "Name");
    EXPECT_EQ(platformVersion->version(), "1");
    EXPECT_EQ(platformVersion->buildVersion(), "10");
    EXPECT_EQ(platformVersion->sourceVersion(), "100");
}
