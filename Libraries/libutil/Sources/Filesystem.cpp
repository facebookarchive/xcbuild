/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

#include <unordered_set>
#include <sstream>

using libutil::Filesystem;
using libutil::FSUtil;

bool Filesystem::
enumerateRecursive(
    std::string const &path,
    std::function<bool(std::string const &)> const &cb) const
{
    this->enumerateDirectory(path, [&](std::string const &filename) -> void {
        std::string full = path + "/" + filename;
        cb(full);
    });

    this->enumerateDirectory(path, [&](std::string const &filename) -> void {
        std::string full = path + "/" + filename;
        if (this->isDirectory(full) && !this->isSymbolicLink(full)) {
            this->enumerateRecursive(full, cb);
        }
    });

    return true;
}

ext::optional<std::string> Filesystem::
findFile(std::string const &name, std::vector<std::string> const &paths) const
{
    if (name.empty()) {
        return ext::nullopt;
    }

    for (auto const &path : paths) {
        std::string filePath = path + "/" + name;
        if (this->exists(filePath)) {
            return FSUtil::NormalizePath(filePath);
        }
    }

    return ext::nullopt;
}

ext::optional<std::string> Filesystem::
findExecutable(std::string const &name, std::vector<std::string> const &paths) const
{
    ext::optional<std::string> exePath = findFile(name, paths);

    if (!exePath) {
        return ext::nullopt;
    }

    if (this->isExecutable(*exePath)) {
        return FSUtil::NormalizePath(*exePath);
    }

    return ext::nullopt;
}

