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
using libutil::Filesystem;

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

TEST(MemoryFilesystem, Type)
{
    auto filesystem = BasicFilesystem();
    EXPECT_EQ(filesystem.type("/"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/dir2"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/dir2/dir3"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/file1"), Filesystem::Type::File);
    EXPECT_EQ(filesystem.type("/dir2/file2"), Filesystem::Type::File);
    EXPECT_EQ(filesystem.type("/invalid"), ext::nullopt);
    EXPECT_EQ(filesystem.type("/invalid1/invalid2"), ext::nullopt);
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
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);
    EXPECT_FALSE(filesystem.write(Contents("new"), "/dir1"));
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);

    /* Can't write in nonexistent directory. */
    EXPECT_FALSE(filesystem.exists("/invalid/new"));
    EXPECT_FALSE(filesystem.write(Contents("new"), "/invalid/new"));
    EXPECT_FALSE(filesystem.exists("/invalid/new"));
}

TEST(MemoryFilesystem, CopyFile)
{
    std::vector<uint8_t> contents;
    auto filesystem = BasicFilesystem();

    /* Must copy to a real path. */
    EXPECT_FALSE(filesystem.copyFile("/file1", "/invalid/file1"));

    /* Can't copy over a directory, even an empty one. */
    EXPECT_FALSE(filesystem.copyFile("/file1", "/dir1"));
    EXPECT_FALSE(filesystem.copyFile("/file1", "/dir2/dir3"));

    /* Can copy to a new path. */
    contents.clear();
    EXPECT_TRUE(filesystem.copyFile("/file1", "/copied"));
    EXPECT_TRUE(filesystem.read(&contents, "/copied"));
    EXPECT_EQ(contents, Contents("one"));

    /* Can copy over an existing file. */
    contents.clear();
    EXPECT_TRUE(filesystem.copyFile("/file1", "/dir1/file2"));
    EXPECT_TRUE(filesystem.read(&contents, "/dir1/file2"));
    EXPECT_EQ(contents, Contents("one"));
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

TEST(MemoryFilesystem, CreateDirectory)
{
    auto filesystem = BasicFilesystem();

    /* Create existing directory, non-recursive. */
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);
    EXPECT_TRUE(filesystem.createDirectory("/dir1", false));
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);

    /* Create existing directory, recursive. */
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);
    EXPECT_TRUE(filesystem.createDirectory("/dir1", true));
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);

    /* Create new directory, non-recursive. */
    EXPECT_FALSE(filesystem.exists("/new1"));
    EXPECT_TRUE(filesystem.createDirectory("/new1", false));
    EXPECT_EQ(filesystem.type("/new1"), Filesystem::Type::Directory);

    /* Create new directory, recursive. */
    EXPECT_FALSE(filesystem.exists("/new2"));
    EXPECT_TRUE(filesystem.createDirectory("/new2", true));
    EXPECT_EQ(filesystem.type("/new2"), Filesystem::Type::Directory);

    /* Create new subdirectory, non-recursive. */
    EXPECT_FALSE(filesystem.exists("/dir1/new1"));
    EXPECT_TRUE(filesystem.createDirectory("/dir1/new1", false));
    EXPECT_EQ(filesystem.type("/dir1/new1"), Filesystem::Type::Directory);

    /* Create new subdirectory, recursive. */
    EXPECT_FALSE(filesystem.exists("/dir1/new2"));
    EXPECT_TRUE(filesystem.createDirectory("/dir1/new2", true));
    EXPECT_EQ(filesystem.type("/dir1/new2"), Filesystem::Type::Directory);

    /* Can't create nested directories when non-recursive. */
    EXPECT_FALSE(filesystem.exists("/invalid/new1"));
    EXPECT_FALSE(filesystem.createDirectory("/invalid/new1", false));
    EXPECT_EQ(filesystem.type("/invalid/new1"), ext::nullopt);
    EXPECT_EQ(filesystem.type("/invalid"), ext::nullopt);

    /* Can create nested directories when recursive. */
    EXPECT_FALSE(filesystem.exists("/invalid/new2"));
    EXPECT_TRUE(filesystem.createDirectory("/invalid/new2", true));
    EXPECT_EQ(filesystem.type("/invalid/new2"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/invalid"), Filesystem::Type::Directory);
}

TEST(MemoryFilesystem, ReadDirectory)
{
    auto filesystem = BasicFilesystem();

    std::vector<std::string> files;
    auto accumulate = [&files](std::string const &name) {
        files.push_back(name);
    };

    /* List root contents, non-recursive. */
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/", false, accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file1", "dir1", "dir2" }));

    /* List root contents, recursive. */
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/", true, accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file1", "dir1", "dir2", "dir1/file2", "dir2/file2", "dir2/dir3" }));

    /* List file. */
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/dir1", false, accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file2" }));
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/dir1", true, accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file2" }));

    /* List files and directories. */
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/dir2", false, accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file2", "dir3" }));
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/dir2", true, accumulate));
    EXPECT_EQ(files, std::vector<std::string>({ "file2", "dir3" }));

    /* List subdirectory. */
    files.clear();
    EXPECT_TRUE(filesystem.readDirectory("/dir2/dir3", false, accumulate));
    EXPECT_EQ(files, std::vector<std::string>());

    /* Can't list file. */
    files.clear();
    EXPECT_FALSE(filesystem.readDirectory("/file1", false, accumulate));
    EXPECT_EQ(files, std::vector<std::string>());
    files.clear();
    EXPECT_FALSE(filesystem.readDirectory("/file1", true, accumulate));
    EXPECT_EQ(files, std::vector<std::string>());

    /* Can't list nonexistent directory. */
    files.clear();
    EXPECT_FALSE(filesystem.readDirectory("/invalid", false, accumulate));
    EXPECT_EQ(files, std::vector<std::string>());
    files.clear();
    EXPECT_FALSE(filesystem.readDirectory("/invalid", true, accumulate));
    EXPECT_EQ(files, std::vector<std::string>());
}

TEST(MemoryFilesystem, CopyDirectory)
{
    auto filesystem = BasicFilesystem();

    /* Must copy into a real path. */
    EXPECT_FALSE(filesystem.copyDirectory("/dir1", "/invalid/dir1", false));

    /* Can copy into a new path non-recursive. */
    EXPECT_TRUE(filesystem.copyDirectory("/dir1", "/copied1", false));
    EXPECT_EQ(filesystem.type("/copied1"), Filesystem::Type::Directory);

    /* Cannot overwrite non-empty directory when non-recursive. */
    EXPECT_FALSE(filesystem.copyDirectory("/dir1", "/dir2", false));

    /* Can overwrite empty directory when non-recursive. */
    EXPECT_TRUE(filesystem.copyDirectory("/dir1", "/dir2/dir3", false));
    EXPECT_EQ(filesystem.type("/dir2/dir3"), Filesystem::Type::Directory);

    /* Can copy into a new path recursive. */
    EXPECT_TRUE(filesystem.copyDirectory("/dir1", "/copied1", true));
    EXPECT_EQ(filesystem.type("/copied1"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/copied1/file2"), Filesystem::Type::File);

    /* Can overwrite empty directory when recursive. */
    EXPECT_TRUE(filesystem.copyDirectory("/dir1", "/dir2/dir3", true));
    EXPECT_EQ(filesystem.type("/dir2/dir3"), Filesystem::Type::Directory);
    EXPECT_EQ(filesystem.type("/dir2/dir3/file2"), Filesystem::Type::File);

    /* Can overwrite non-empty directory when recursive. */
    EXPECT_TRUE(filesystem.copyDirectory("/dir1", "/dir2", true));
    EXPECT_EQ(filesystem.type("/dir2/dir3"), ext::nullopt);
    EXPECT_EQ(filesystem.type("/dir2/file2"), Filesystem::Type::File);
}

TEST(MemoryFilesystem, RemoveDirectory)
{
    auto filesystem = BasicFilesystem();

    /* Can remove empty directory when non-recursive. */
    EXPECT_TRUE(filesystem.removeDirectory("/dir2/dir3", false));
    EXPECT_EQ(filesystem.type("/dir2/dir3"), ext::nullopt);

    /* Can't remove non-empty directory when non-recursive. */
    EXPECT_FALSE(filesystem.removeDirectory("/dir1", false));
    EXPECT_EQ(filesystem.type("/dir1/file2"), Filesystem::Type::File);
    EXPECT_EQ(filesystem.type("/dir1"), Filesystem::Type::Directory);

    /* Can't remove non-empty directory when non-recursive. */
    EXPECT_TRUE(filesystem.removeDirectory("/dir1", true));
    EXPECT_EQ(filesystem.type("/dir1/file2"), ext::nullopt);
    EXPECT_EQ(filesystem.type("/dir1"), ext::nullopt);

    /* Can't remove files. */
    EXPECT_TRUE(filesystem.exists("/file1"));
    EXPECT_FALSE(filesystem.removeDirectory("/file1", false));
    EXPECT_FALSE(filesystem.removeDirectory("/file1", true));
    EXPECT_TRUE(filesystem.exists("/file1"));

    /* Can't remove invalid paths. */
    EXPECT_FALSE(filesystem.removeDirectory("/invalid", false));
    EXPECT_FALSE(filesystem.removeDirectory("/invalid", true));
}

TEST(MemoryFilesystem, ResolvePath)
{
    auto filesystem = BasicFilesystem();
    EXPECT_EQ(filesystem.resolvePath("/"), "/");
    EXPECT_EQ(filesystem.resolvePath("//"), "/");
    EXPECT_EQ(filesystem.resolvePath("/./"), "/");
    EXPECT_EQ(filesystem.resolvePath("//file1"), "/file1");
    EXPECT_EQ(filesystem.resolvePath("/dir1"), "/dir1");
    EXPECT_EQ(filesystem.resolvePath("/dir1/"), "/dir1");
    EXPECT_EQ(filesystem.resolvePath("/dir1/.."), "/");
    EXPECT_EQ(filesystem.resolvePath("/dir1//file2"), "/dir1/file2");
    EXPECT_EQ(filesystem.resolvePath("/dir2/.././dir1/file2"), "/dir1/file2");
}

