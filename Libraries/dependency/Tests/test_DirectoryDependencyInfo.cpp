/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <dependency/DirectoryDependencyInfo.h>
#include <libutil/MemoryFilesystem.h>

using dependency::DirectoryDependencyInfo;
using dependency::DependencyInfo;
using libutil::MemoryFilesystem;

TEST(DirectoryDependencyInfo, Deserialize)
{
    MemoryFilesystem filesystem = MemoryFilesystem({
        MemoryFilesystem::Entry::Directory("root", {
            MemoryFilesystem::Entry::File("file1", { }),
            MemoryFilesystem::Entry::File("file2", { }),
            MemoryFilesystem::Entry::Directory("dir", {
                MemoryFilesystem::Entry::File("file3", { }),
            }),
        }),
    });

    auto info = DirectoryDependencyInfo::Deserialize(&filesystem, "/root");
    ASSERT_TRUE(info);
    EXPECT_EQ(info->dependencyInfo().inputs(), std::vector<std::string>({
        "/root/file1",
        "/root/file2",
        "/root/dir/file3",
        "/root/dir",
    }));
    EXPECT_TRUE(info->dependencyInfo().outputs().empty());
}

TEST(DirectoryDependencyInfo, File)
{
    MemoryFilesystem filesystem = MemoryFilesystem({
        MemoryFilesystem::Entry::File("file1", { }),
    });

    auto info = DirectoryDependencyInfo::Deserialize(&filesystem, "/file");
    ASSERT_EQ(info, ext::nullopt);
}
