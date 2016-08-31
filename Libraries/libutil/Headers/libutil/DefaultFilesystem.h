/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_DefaultFilesystem_h
#define __libutil_DefaultFilesystem_h

#include <libutil/Filesystem.h>

namespace libutil {

class DefaultFilesystem : public Filesystem {
public:
    virtual bool exists(std::string const &path) const;

public:
    virtual bool isDirectory(std::string const &path) const;
    virtual bool isSymbolicLink(std::string const &path) const;

public:
    virtual bool isReadable(std::string const &path) const;
    virtual bool isWritable(std::string const &path) const;
    virtual bool isExecutable(std::string const &path) const;

public:
    virtual bool createFile(std::string const &path);
    virtual bool createDirectory(std::string const &path);

public:
    virtual bool read(std::vector<uint8_t> *contents, std::string const &path, size_t offset = 0, ext::optional<size_t> length = ext::nullopt) const;
    virtual bool write(std::vector<uint8_t> const &contents, std::string const &path);
    virtual ext::optional<std::string> readSymbolicLink(std::string const &path) const;
    virtual bool writeSymbolicLink(std::string const &target, std::string const &path);

public:
    virtual bool removeFile(std::string const &path);

public:
    virtual std::string resolvePath(std::string const &path) const;

public:
    virtual bool enumerateDirectory(
        std::string const &path,
        std::function<void(std::string const &)> const &cb) const;
};

}

#endif  // !__libutil_DefaultFilesystem_h
