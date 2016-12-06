/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/DefaultFilesystem.h>
#include <libutil/FSUtil.h>

#include <stack>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <copyfile.h>
#endif

using libutil::DefaultFilesystem;
using libutil::Filesystem;
using libutil::Permissions;

bool DefaultFilesystem::
exists(std::string const &path) const
{
    return ::access(path.c_str(), F_OK) == 0;
}

ext::optional<Filesystem::Type> DefaultFilesystem::
type(std::string const &path) const
{
    struct stat st;
    if (::lstat(path.c_str(), &st) < 0) {
        return ext::nullopt;
    }

    if (S_ISREG(st.st_mode)) {
        return Type::File;
    } else if (S_ISLNK(st.st_mode)) {
        return Type::SymbolicLink;
    } else if (S_ISDIR(st.st_mode)) {
        return Type::Directory;
    } else {
        /* Unsupported file type, e.g. character or block device. */
        return ext::nullopt;
    }
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

static Permissions
ModePermissions(mode_t mode)
{
    Permissions permissions;
    permissions.flag(Permissions::Flag::Sticky, (mode & S_ISVTX) != 0);
    permissions.flag(Permissions::Flag::SetUserID, (mode & S_ISUID) != 0);
    permissions.flag(Permissions::Flag::SetGroupID, (mode & S_ISGID) != 0);
    permissions.user(Permissions::Permission::Read, (mode & S_IRUSR) != 0);
    permissions.user(Permissions::Permission::Write, (mode & S_IWUSR) != 0);
    permissions.user(Permissions::Permission::Execute, (mode & S_IXUSR) != 0);
    permissions.group(Permissions::Permission::Read, (mode & S_IRGRP) != 0);
    permissions.group(Permissions::Permission::Write, (mode & S_IWGRP) != 0);
    permissions.group(Permissions::Permission::Execute, (mode & S_IXGRP) != 0);
    permissions.other(Permissions::Permission::Read, (mode & S_IROTH) != 0);
    permissions.other(Permissions::Permission::Write, (mode & S_IWOTH) != 0);
    permissions.other(Permissions::Permission::Execute, (mode & S_IXOTH) != 0);
    return permissions;
}

ext::optional<Permissions> DefaultFilesystem::
readFilePermissions(std::string const &path) const
{
    struct stat st;
    if (::stat(path.c_str(), &st) < 0) {
        return ext::nullopt;
    }

    return ModePermissions(st.st_mode);
}

ext::optional<Permissions> DefaultFilesystem::
readSymbolicLinkPermissions(std::string const &path) const
{
    struct stat st;
    if (::lstat(path.c_str(), &st) < 0) {
        return ext::nullopt;
    }

    return ModePermissions(st.st_mode);
}

ext::optional<Permissions> DefaultFilesystem::
readDirectoryPermissions(std::string const &path) const
{
    struct stat st;
    if (::stat(path.c_str(), &st) < 0) {
        return ext::nullopt;
    }

    return ModePermissions(st.st_mode);
}

static mode_t
PermissionsMode(Permissions permissions)
{
    mode_t mode = 0;
    mode |= (permissions.flag(Permissions::Flag::Sticky) ? S_ISVTX : 0);
    mode |= (permissions.flag(Permissions::Flag::SetUserID) ? S_ISUID : 0);
    mode |= (permissions.flag(Permissions::Flag::SetGroupID) ? S_ISUID : 0);
    mode |= (permissions.user(Permissions::Permission::Read) ? S_IRUSR : 0);
    mode |= (permissions.user(Permissions::Permission::Write) ? S_IWUSR : 0);
    mode |= (permissions.user(Permissions::Permission::Execute) ? S_IXUSR : 0);
    mode |= (permissions.group(Permissions::Permission::Read) ? S_IRUSR : 0);
    mode |= (permissions.group(Permissions::Permission::Write) ? S_IWUSR : 0);
    mode |= (permissions.group(Permissions::Permission::Execute) ? S_IXUSR : 0);
    mode |= (permissions.other(Permissions::Permission::Read) ? S_IRUSR : 0);
    mode |= (permissions.other(Permissions::Permission::Write) ? S_IWUSR : 0);
    mode |= (permissions.other(Permissions::Permission::Execute) ? S_IXUSR : 0);
    return mode;
}

bool DefaultFilesystem::
writeFilePermissions(std::string const &path, Permissions::Operation operation, Permissions permissions)
{
    ext::optional<Permissions> updated = this->readFilePermissions(path);
    if (!updated) {
        return false;
    }

    updated->combine(operation, permissions);
    mode_t mode = PermissionsMode(*updated);

    if (::chmod(path.c_str(), mode) != 0) {
        return false;
    }

    return true;
}

bool DefaultFilesystem::
writeSymbolicLinkPermissions(std::string const &path, Permissions::Operation operation, Permissions permissions)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    ext::optional<Permissions> updated = this->readSymbolicLinkPermissions(path);
    if (!updated) {
        return false;
    }

    updated->combine(operation, permissions);
    mode_t mode = PermissionsMode(*updated);

    if (::lchmod(path.c_str(), mode) != 0) {
        return false;
    }

    return true;
#else
    /* Symbolic links have no permissions on most filesystems. */
    return true;
#endif
}

bool DefaultFilesystem::
writeDirectoryPermissions(std::string const &path, Permissions::Operation operation, Permissions permissions, bool recursive)
{
    ext::optional<Permissions> updated = this->readDirectoryPermissions(path);
    if (!updated) {
        return false;
    }

    updated->combine(operation, permissions);
    mode_t mode = PermissionsMode(*updated);

    if (::chmod(path.c_str(), mode) != 0) {
        return false;
    }

    if (recursive) {
        bool success = true;

        success &= this->readDirectory(path, recursive, [this, &path, &operation, &permissions, &success](std::string const &name) {
            std::string full = path + "/" + name;

            ext::optional<Type> type = this->type(full);
            if (!type) {
                return false;
            }

            switch (*type) {
                case Type::File:
                    if (!this->writeFilePermissions(full, operation, permissions)) {
                        success = false;
                        return false;
                    }
                    break;
                case Type::Directory:
                    /* Already recursing directory; don't need to recurse again. */
                    if (!this->writeDirectoryPermissions(full, operation, permissions, false)) {
                        success = false;
                        return false;
                    }
                    break;
                case Type::SymbolicLink:
                    if (!this->writeSymbolicLinkPermissions(full, operation, permissions)) {
                        success = false;
                        return false;
                    }
                    break;
            }

            return true;
        });

        if (!success) {
            return false;
        }
    }

    return true;
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
read(std::vector<uint8_t> *contents, std::string const &path, size_t offset, ext::optional<size_t> length) const
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

    if (length) {
        if (offset + *length > size) {
            std::fclose(fp);
            return false;
        }

        size = *length;
    }

    if (std::fseek(fp, offset, SEEK_SET) != 0) {
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

bool DefaultFilesystem::
copyFile(std::string const &from, std::string const &to)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    ext::optional<Type> fromType = this->type(from);
    if (fromType != Type::File && fromType != Type::SymbolicLink) {
        return false;
    }

    ext::optional<Type> toType = this->type(to);
    if (toType) {
        switch (*toType) {
            case Type::File:
                if (!this->removeFile(to)) {
                    return false;
                }
                break;
            case Type::SymbolicLink:
                if (!this->removeSymbolicLink(to)) {
                    return false;
                }
                break;
            case Type::Directory:
                return false;
        }
    }

    copyfile_state_t state = ::copyfile_state_alloc();
    copyfile_flags_t flags = COPYFILE_ALL | COPYFILE_NOFOLLOW;
    if (::copyfile(from.c_str(), to.c_str(), state, flags)) {
        return false;
    }
    ::copyfile_state_free(state);

    return true;
#else
    return Filesystem::copyFile(from, to);
#endif
}

bool DefaultFilesystem::
removeFile(std::string const &path)
{
    if (::unlink(path.c_str()) < 0) {
        return false;
    }
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
    if (::symlink(target.c_str(), path.c_str()) != 0) {
        return false;
    }

    return true;
}

bool DefaultFilesystem::
copySymbolicLink(std::string const &from, std::string const &to)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    if (this->type(from) != Type::SymbolicLink) {
        return false;
    }

    if (this->type(to) == Type::SymbolicLink) {
        if (!this->removeSymbolicLink(to)) {
            return false;
        }
    }

    copyfile_state_t state = ::copyfile_state_alloc();
    copyfile_flags_t flags = COPYFILE_ALL | COPYFILE_NOFOLLOW;
    if (::copyfile(from.c_str(), to.c_str(), state, flags)) {
        return false;
    }
    ::copyfile_state_free(state);

    return true;
#else
    return Filesystem::copySymbolicLink(from, to);
#endif
}

bool DefaultFilesystem::
removeSymbolicLink(std::string const &path)
{
    if (this->type(path) == Type::SymbolicLink) {
        if (::unlink(path.c_str()) != 0) {
            return false;
        }
    }

    return true;
}

bool DefaultFilesystem::
createDirectory(std::string const &path, bool recursive)
{
    /* Get and re-set current mode mask. */
    mode_t mask = ::umask(0);
    ::umask(mask);

    /* Mode is most allowed by mask. */
    mode_t mode = (S_IRWXU | S_IRWXG | S_IRWXO) & ~mask;

    if (recursive) {
        std::string current = path;
        std::stack<std::string> create;

        /* Build up list of directories to create. */
        while (this->type(current) != Type::Directory && current != "") {
            create.push(current);
            current = FSUtil::GetDirectoryName(current);
        }

        /* Create intermediate directories. */
        while (!create.empty()) {
            std::string const &directory = create.top();

            if (::mkdir(directory.c_str(), mode) != 0) {
                return false;
            }

            create.pop();
        }
    } else {
        if (::mkdir(path.c_str(), mode) != 0) {
            return false;
        }
    }

    return true;
}

bool DefaultFilesystem::
readDirectory(std::string const &path, bool recursive, std::function<void(std::string const &)> const &cb) const
{
    std::function<bool(std::string const &, ext::optional<std::string> const &)> process =
        [this, &recursive, &cb, &process](std::string const &absolute, ext::optional<std::string> const &relative) -> bool {
        DIR *dp = ::opendir(absolute.c_str());
        if (dp == NULL) {
            return false;
        }

        /* Report children. */
        while (struct dirent *entry = ::readdir(dp)) {
            if (::strcmp(entry->d_name, ".") == 0 || ::strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::string path = (relative ? *relative + "/" + entry->d_name : entry->d_name);
            cb(path);
        }

        /* Process subdirectories. */
        if (recursive) {
            ::rewinddir(dp);

            while (struct dirent *entry = ::readdir(dp)) {
                if (::strcmp(entry->d_name, ".") == 0 || ::strcmp(entry->d_name, "..") == 0) {
                    continue;
                }

                std::string full = absolute + "/" + entry->d_name;

                if (this->type(full) == Type::Directory) {
                    std::string path = (relative ? *relative + "/" + entry->d_name : entry->d_name);
                    if (!process(full, path)) {
                        ::closedir(dp);
                        return false;
                    }
                }
            }
        }

        ::closedir(dp);
        return true;
    };

    return process(path, ext::nullopt);
}

bool DefaultFilesystem::
copyDirectory(std::string const &from, std::string const &to, bool recursive)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    if (this->type(from) != Type::Directory) {
        return false;
    }

    if (this->type(to) == Type::Directory) {
        if (!this->removeDirectory(to, recursive)) {
            return false;
        }
    }

    copyfile_state_t state = ::copyfile_state_alloc();
    copyfile_flags_t flags = COPYFILE_ALL | COPYFILE_NOFOLLOW | (recursive ? COPYFILE_RECURSIVE : 0);
    if (::copyfile(from.c_str(), to.c_str(), state, flags)) {
        return false;
    }
    ::copyfile_state_free(state);

    return true;
#else
    return Filesystem::copyDirectory(from, to, recursive);
#endif
}

bool DefaultFilesystem::
removeDirectory(std::string const &path, bool recursive)
{
    if (recursive) {
        bool success = true;

        success &= this->readDirectory(path, recursive, [this, &path, &success](std::string const &name) {
            std::string full = path + "/" + name;

            ext::optional<Type> type = this->type(full);
            if (!type) {
                return false;
            }

            switch (*type) {
                case Type::File:
                    if (!this->removeFile(full)) {
                        success = false;
                        return false;
                    }
                    break;
                case Type::SymbolicLink:
                    if (!this->removeSymbolicLink(full)) {
                        success = false;
                        return false;
                    }
                    break;
                case Type::Directory:
                    if (!this->removeDirectory(full, false)) {
                        success = false;
                        return false;
                    }
                    break;
            }

            return true;
        });

        if (!success) {
            return false;
        }
    }

    if (::rmdir(path.c_str()) != 0) {
        return false;
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
