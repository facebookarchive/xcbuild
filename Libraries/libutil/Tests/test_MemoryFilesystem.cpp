/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <libutil/MemoryFilesystem.h>

using libutil::MemoryFilesystem;

static std::vector<uint8_t>
Contents(std::string const &string)
{
    return std::vector<uint8_t>(string.begin(), string.end());
}

static MemoryFilesystem
BasicFilesystem()
{
    return MemoryFilesystem({
        MemoryFilesystem::Entry::File("file1", Contents("one")),
        MemoryFilesystem::Entry::Directory("dir1", {
            MemoryFilesystem::Entry::File("file2", Contents("two1")),
        }),
        MemoryFilesystem::Entry::Directory("dir2", {
            MemoryFilesystem::Entry::File("file2", Contents("two2")),
            MemoryFilesystem::Entry::Directory("dir3", { }),
        }),
    });
}

TEST(MemoryFilesystem, Exists)
{
    auto filesystem = BasicFilesystem();
    EXPECT_TRUE(filesystem.exists("/"));
    EXPECT_TRUE(filesystem.exists("/dir1"));
    EXPECT_TRUE(filesystem.exists("/dir2"));
    EXPECT_TRUE(filesystem.exists("/dir2/dir3"));
    EXPECT_TRUE(filesystem.exists("/file1"));
    EXPECT_TRUE(filesystem.exists("/dir2/file2"));
    EXPECT_FALSE(filesystem.exists("/invalid"));
    EXPECT_FALSE(filesystem.exists("/dir1/invalid"));
    EXPECT_FALSE(filesystem.exists("/invalid1/invalid2"));
}

TEST(MemoryFilesystem, IsDirectory)
{
    auto filesystem = BasicFilesystem();
    EXPECT_TRUE(filesystem.isDirectory("/"));
    EXPECT_TRUE(filesystem.isDirectory("/dir1"));
    EXPECT_TRUE(filesystem.isDirectory("/dir2"));
    EXPECT_TRUE(filesystem.isDirectory("/dir2/dir3"));
    EXPECT_FALSE(filesystem.isDirectory("/file1"));
    EXPECT_FALSE(filesystem.isDirectory("/dir2/file2"));
    EXPECT_FALSE(filesystem.isDirectory("/invalid"));
    EXPECT_FALSE(filesystem.isDirectory("/invalid1/invalid2"));
}

TEST(MemoryFilesystem, IsSymbolicLink)
{
    auto filesystem = BasicFilesystem();
    EXPECT_FALSE(filesystem.isSymbolicLink("/"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/file1"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/dir1"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/dir2"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/dir2/file2"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/dir2/dir3"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/invalid"));
    EXPECT_FALSE(filesystem.isSymbolicLink("/invalid1/invalid2"));
}

TEST(MemoryFilesystem, IsReadable)
{
    auto filesystem = BasicFilesystem();
    EXPECT_TRUE(filesystem.isReadable("/"));
    EXPECT_TRUE(filesystem.isReadable("/dir1"));
    EXPECT_TRUE(filesystem.isReadable("/dir2"));
    EXPECT_TRUE(filesystem.isReadable("/dir2/dir3"));
    EXPECT_TRUE(filesystem.isReadable("/file1"));
    EXPECT_TRUE(filesystem.isReadable("/dir2/file2"));
    EXPECT_FALSE(filesystem.isReadable("/invalid"));
    EXPECT_FALSE(filesystem.isReadable("/dir1/invalid"));
    EXPECT_FALSE(filesystem.isReadable("/invalid1/invalid2"));
}

TEST(MemoryFilesystem, IsWritable)
{
    auto filesystem = BasicFilesystem();
    EXPECT_TRUE(filesystem.isWritable("/"));
    EXPECT_TRUE(filesystem.isWritable("/dir1"));
    EXPECT_TRUE(filesystem.isWritable("/dir2"));
    EXPECT_TRUE(filesystem.isWritable("/dir2/dir3"));
    EXPECT_TRUE(filesystem.isWritable("/file1"));
    EXPECT_TRUE(filesystem.isWritable("/dir2/file2"));
    EXPECT_FALSE(filesystem.isWritable("/invalid"));
    EXPECT_FALSE(filesystem.isWritable("/dir1/invalid"));
    EXPECT_FALSE(filesystem.isWritable("/invalid1/invalid2"));
}

TEST(MemoryFilesystem, IsExecutable)
{
    auto filesystem = BasicFilesystem();
    EXPECT_TRUE(filesystem.isExecutable("/"));
    EXPECT_TRUE(filesystem.isExecutable("/dir1"));
    EXPECT_TRUE(filesystem.isExecutable("/dir2"));
    EXPECT_TRUE(filesystem.isExecutable("/dir2/dir3"));
    EXPECT_TRUE(filesystem.isExecutable("/file1"));
    EXPECT_TRUE(filesystem.isExecutable("/dir2/file2"));
    EXPECT_FALSE(filesystem.isExecutable("/invalid"));
    EXPECT_FALSE(filesystem.isExecutable("/dir1/invalid"));
    EXPECT_FALSE(filesystem.isExecutable("/invalid1/invalid2"));
}

TEST(MemoryFilesystem, CreateFile)
{
    auto filesystem = BasicFilesystem();

    /* Create file. */
    EXPECT_FALSE(filesystem.exists("/new1"));
    EXPECT_TRUE(filesystem.createFile("/new1"));
    EXPECT_TRUE(filesystem.exists("/new1"));

    /* Create existing file. */
    EXPECT_TRUE(filesystem.exists("/file1"));
    EXPECT_TRUE(filesystem.createFile("/file1"));
    EXPECT_TRUE(filesystem.exists("/file1"));

    /* Create file in directory. */
    EXPECT_FALSE(filesystem.exists("/dir1/new1"));
    EXPECT_TRUE(filesystem.createFile("/dir1/new1"));
    EXPECT_TRUE(filesystem.exists("/dir1/new1"));

    /* Can't create file in nonexistent directory. */
    EXPECT_FALSE(filesystem.exists("/invalid/new1"));
    EXPECT_FALSE(filesystem.createFile("/invalid/new1"));
    EXPECT_FALSE(filesystem.exists("/invalid/new1"));
}

TEST(MemoryFilesystem, CreateDirectory)
{
    auto filesystem = BasicFilesystem();

    /* Create existing directory. */
    EXPECT_TRUE(filesystem.isDirectory("/dir1"));
    EXPECT_TRUE(filesystem.createDirectory("/dir1"));
    EXPECT_TRUE(filesystem.isDirectory("/dir1"));

    /* Create new directory. */
    EXPECT_FALSE(filesystem.exists("/new1"));
    EXPECT_TRUE(filesystem.createDirectory("/new1"));
    EXPECT_TRUE(filesystem.isDirectory("/new1"));

    /* Create new subdirectory. */
    EXPECT_FALSE(filesystem.exists("/dir1/new1"));
    EXPECT_TRUE(filesystem.createDirectory("/dir1/new1"));
    EXPECT_TRUE(filesystem.isDirectory("/dir1/new1"));

    /* Create nested directories. */
    EXPECT_FALSE(filesystem.exists("/invalid/new1"));
    EXPECT_TRUE(filesystem.createDirectory("/invalid/new1"));
    EXPECT_TRUE(filesystem.isDirectory("/invalid/new1"));
    EXPECT_TRUE(filesystem.isDirectory("/invalid"));
}

TEST(MemoryFilesystem, RemoveFile)
{
    auto filesystem = BasicFilesystem();

    EXPECT_TRUE(filesystem.exists("/file1"));
    EXPECT_TRUE(filesystem.removeFile("/file1"));
    EXPECT_FALSE(filesystem.exists("/file1"));

    EXPECT_TRUE(filesystem.exists("/dir1/file2"));
    EXPECT_TRUE(filesystem.removeFile("/dir1/file2"));
    EXPECT_FALSE(filesystem.exists("/dir1/file2"));

    EXPECT_FALSE(filesystem.removeFile("/dir1"));
    EXPECT_FALSE(filesystem.removeFile("/dir2/dir3"));
    EXPECT_FALSE(filesystem.removeFile("/invalid"));
}

TEST(MemoryFilesystem, Read)
{
    auto filesystem = BasicFilesystem();
    std::vector<uint8_t> contents;

    /* Read file. */
    contents.clear();
    EXPECT_TRUE(filesystem.read(&contents, "/file1"));
    EXPECT_EQ(contents, Contents("one"));

    /* Read file in subdirectory. */
    contents.clear();
    EXPECT_TRUE(filesystem.read(&contents, "/dir1/file2"));
    EXPECT_EQ(contents, Contents("two1"));

    /* Read file in subdirectory. */
    contents.clear();
    EXPECT_TRUE(filesystem.read(&contents, "/dir2/file2"));
    EXPECT_EQ(contents, Contents("two2"));

    /* Can't read root. */
    contents.clear();
    EXPECT_FALSE(filesystem.read(&contents, "/"));
    EXPECT_EQ(contents, Contents(""));

    /* Can't read directory. */
    contents.clear();
    EXPECT_FALSE(filesystem.read(&contents, "/dir1"));
    EXPECT_EQ(contents, Contents(""));

    /* Can't read nonexistent file. */
    contents.clear();
    EXPECT_FALSE(filesystem.read(&contents, "/invalid"));
    EXPECT_EQ(contents, Contents(""));
}

TEST(MemoryFilesystem, Write)
{
    auto filesystem = BasicFilesystem();
    std::vector<uint8_t> contents;

    /* Write new file. */
    EXPECT_FALSE(filesystem.exists("/new"));
    EXPECT_TRUE(filesystem.write(Contents("new"), "/new"));
    contents.clear();
    EXPECT_TRUE(filesystem.read(&contents, "/new"));
    EXPECT_EQ(contents, Contents("new"));

    /* Rewrite existing file. */
    contents.clear();
    EXPECT_TRUE(filesystem.read(&contents, "/file1"));
    EXPECT_EQ(contents, Contents("one"));
    EXPECT_TRUE(filesystem.write(Contents("new"), "/file1"));
    contents.clear();
    EXPECT_TRUE(filesystem.read(&contents, "/file1"));
    EXPECT_EQ(contents, Contents("new"));

    /* Can't write to a directory. */
    EXPECT_TRUE(filesystem.isDirectory("/dir1"));
    EXPECT_FALSE(filesystem.write(Contents("new"), "/dir1"));
    EXPECT_TRUE(filesystem.isDirectory("/dir1"));

    /* Can't write in nonexistent directory. */
    EXPECT_FALSE(filesystem.exists("/invalid/new"));
    EXPECT_FALSE(filesystem.write(Contents("new"), "/invalid/new"));
    EXPECT_FALSE(filesystem.exists("/invalid/new"));
}

TEST(MemoryFilesystem, ResolvePath)
{
    auto filesystem = BasicFilesystem();
    EXPECT_EQ(filesystem.resolvePath("/"), "/");
    EXPECT_EQ(filesystem.resolvePath("//"), "/");
    EXPECT_EQ(filesystem.resolvePath("/./"), "/");
    EXPECT_EQ(filesystem.resolvePath("//file1"), "/file1");
    EXPECT_EQ(filesystem.resolvePath("/dir1"), "/dir1");
    EXPECT_EQ(filesystem.resolvePath("/dir1/"), "/dir1/");
    EXPECT_EQ(filesystem.resolvePath("/dir1/.."), "/");
    EXPECT_EQ(filesystem.resolvePath("/dir1//file2"), "/dir1/file2");
    EXPECT_EQ(filesystem.resolvePath("/dir2/.././dir1/file2"), "/dir1/file2");
}

TEST(MemoryFilesystem, EnumerateDirectory)
{
    auto filesystem = BasicFilesystem();

    std::vector<std::string> files;
    auto accumulate = [&files](std::string const &name) {
        files.push_back(name);
        return true;
    };

    /* List root contents. */
    files.clear();
    EXPECT_TRUE(filesystem.enumerateDirectory("/", accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file1", "dir1", "dir2" }));

    /* List file. */
    files.clear();
    EXPECT_TRUE(filesystem.enumerateDirectory("/dir1", accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file2" }));

    /* List files and directories. */
    files.clear();
    EXPECT_TRUE(filesystem.enumerateDirectory("/dir2", accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file2", "dir3" }));

    /* List subdirectory. */
    files.clear();
    EXPECT_TRUE(filesystem.enumerateDirectory("/dir2/dir3", accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ }));

    /* Can't list file. */
    files.clear();
    EXPECT_FALSE(filesystem.enumerateDirectory("/file1", accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ }));

    /* Can't list nonexistent directory. */
    files.clear();
    EXPECT_FALSE(filesystem.enumerateDirectory("/invalid", accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ }));
}

