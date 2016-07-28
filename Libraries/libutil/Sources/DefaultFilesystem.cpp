/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/DefaultFilesystem.h>
#include <libutil/FSUtil.h>

#include <fstream>
#include <iterator>

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>

using libutil::DefaultFilesystem;

bool DefaultFilesystem::
exists(std::string const &path) const
{
    return ::access(path.c_str(), F_OK) == 0;
}

bool DefaultFilesystem::
isDirectory(std::string const &path) const
{
    struct stat st;
    if (::stat(path.c_str(), &st) < 0)
        return false;
    else
        return S_ISDIR(st.st_mode);
}

bool DefaultFilesystem::
isSymbolicLink(std::string const &path) const
{
    struct stat st;
    if (::lstat(path.c_str(), &st) < 0)
        return false;
    else
        return S_ISLNK(st.st_mode);
}

bool DefaultFilesystem::
isReadable(std::string const &path) const
{
    return ::access(path.c_str(), R_OK) == 0;
}

bool DefaultFilesystem::
isWritable(std::string const &path) const
{
    return ::access(path.c_str(), W_OK) == 0;
}

bool DefaultFilesystem::
isExecutable(std::string const &path) const
{
    return ::access(path.c_str(), X_OK) == 0;
}

bool DefaultFilesystem::
createFile(std::string const &path)
{
    if (this->isWritable(path)) {
        return true;
    }

    FILE *fp = std::fopen(path.c_str(), "w");
    if (fp == nullptr) {
        return false;
    }

    std::fclose(fp);
    return true;
}

bool DefaultFilesystem::
createDirectory(std::string const &path)
{
    std::vector<std::string> components;

    std::string current = path;
    while (current != FSUtil::GetDirectoryName(current)) {
        std::string component = FSUtil::GetBaseName(current);
        components.push_back(component);

        current = FSUtil::GetDirectoryName(current);
    }

    for (auto it = components.rbegin(); it != components.rend(); ++it) {
        std::string const &component = *it;
        if (it != components.rbegin()) {
            current += "/";
        }
        current += component;

        if (::mkdir(current.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0 && errno != EEXIST) {
            return false;
        }
    }

    return true;
}

bool DefaultFilesystem::
read(std::vector<uint8_t> *contents, std::string const &path) const
{
    FILE *fp = std::fopen(path.c_str(), "rb");
    if (fp == nullptr) {
        return false;
    }

    if (std::fseek(fp, 0, SEEK_END) != 0) {
        std::fclose(fp);
        return false;
    }

    long size = std::ftell(fp);
    if (size == (long)-1) {
        std::fclose(fp);
        return false;
    }

    if (std::fseek(fp, 0, SEEK_SET) != 0) {
        std::fclose(fp);
        return false;
    }

    *contents = std::vector<uint8_t>(size);

    if (size > 0) {
        if (std::fread(contents->data(), size, 1, fp) != 1) {
            std::fclose(fp);
            return false;
        }
    }

    std::fclose(fp);
    return true;
}

bool DefaultFilesystem::
write(std::vector<uint8_t> const &contents, std::string const &path)
{
    FILE *fp = std::fopen(path.c_str(), "wb");
    if (fp == nullptr) {
        return false;
    }

    size_t size = contents.size();

    if (size > 0) {
        if (std::fwrite(contents.data(), size, 1, fp) != 1) {
            std::fclose(fp);
            return false;
        }
    }

    std::fclose(fp);
    return true;
}

ext::optional<std::string> DefaultFilesystem::
readSymbolicLink(std::string const &path) const
{
    char buffer[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buffer, sizeof(buffer) - 1);
    if (len == -1) {
        return ext::nullopt;
    }

    buffer[len] = '\0';
    return std::string(buffer);
}

bool DefaultFilesystem::
writeSymbolicLink(std::string const &target, std::string const &path)
{
    if (symlink(target.c_str(), path.c_str())) {
        return false;
    }

    return true;
}

bool DefaultFilesystem::
removeFile(std::string const &path)
{
    if (::unlink(path.c_str()) < 0) {
        if (::unlink(path.c_str()) < 0) {
            return false;
        }
    }
    return true;
}

std::string DefaultFilesystem::
resolvePath(std::string const &path) const
{
    char realPath[PATH_MAX + 1];
    if (::realpath(path.c_str(), realPath) == nullptr) {
        return std::string();
    } else {
        return realPath;
    }
}

bool DefaultFilesystem::
enumerateDirectory(
    std::string const &path,
    std::function<void(std::string const &)> const &cb) const
{
    DIR *dp = opendir(path.c_str());
    if (dp == NULL) {
        return false;
    }

    while (struct dirent *entry = readdir(dp)) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            cb(name);
        }
    }

    closedir(dp);
    return true;
}

