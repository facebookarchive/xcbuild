/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_Filesystem_h
#define __libutil_Filesystem_h

#include <functional>
#include <string>
#include <vector>
#include <ext/optional>

namespace libutil {

class Filesystem {
public:
    /*
     * Test if a file exists.
     */
    virtual bool exists(std::string const &path) const = 0;

public:
    /*
     * Test if a path is a directory.
     */
    virtual bool isDirectory(std::string const &path) const = 0;

    /*
     * Test if a path is a symbolic link.
     */
    virtual bool isSymbolicLink(std::string const &path) const = 0;

public:
    /*
     * Test if a file is readable.
     */
    virtual bool isReadable(std::string const &path) const = 0;

    /*
     * Test if a file is writable.
     */
    virtual bool isWritable(std::string const &path) const = 0;

    /*
     * Test if a file is executable.
     */
    virtual bool isExecutable(std::string const &path) const = 0;

public:
    /*
     * Create a file. Succeeds if created or already exists.
     */
    virtual bool createFile(std::string const &path) = 0;

    /*
     * Create a directory. Succeeds if created or already exists.
     */
    virtual bool createDirectory(std::string const &path) = 0;

public:
    /*
     * Read from a file.
     */
    virtual bool read(std::vector<uint8_t> *contents, std::string const &path) const = 0;

    /*
     * Write to a file.
     */
    virtual bool write(std::vector<uint8_t> const &contents, std::string const &path) = 0;

    /*
     * Read the destination of the symbolic link, relative to its containing directory.
     */
    virtual ext::optional<std::string> readSymbolicLink(std::string const &path) const = 0;

    /*
     * Write a symbolic link to a target, relative to the containing directory.
     */
    virtual bool writeSymbolicLink(std::string const &target, std::string const &path) = 0;

public:
    /*
     * Delete a file.
     */
    virtual bool removeFile(std::string const &path) = 0;

public:
    /*
     * Resolves and normalizes a path through symbolic links.
     */
    virtual std::string resolvePath(std::string const &path) const = 0;

public:
    /*
     * Enumerate contents of a directory.
     */
    virtual bool enumerateDirectory(
        std::string const &path,
        std::function<void(std::string const &)> const &cb) const = 0;

    /*
     * Enumerate the contents of a directory recursively.
     */
    bool enumerateRecursive(
        std::string const &path,
        std::function<bool(std::string const &)> const &cb) const;

public:
    /*
     * Finds a file in the given directories.
     */
    ext::optional<std::string> findFile(std::string const &name, std::vector<std::string> const &paths) const;

    /*
     * Finds an executable in the given directories.
     */
    ext::optional<std::string> findExecutable(std::string const &name, std::vector<std::string> const &paths) const;
};

}

#endif  // !__libutil_Filesystem_h
