/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_FSUtil_h
#define __libutil_FSUtil_h

#include <functional>
#include <string>
#include <vector>

namespace libutil {

class FSUtil {
public:
    static bool TestForPresence(std::string const &path);
    static bool TestForRead(std::string const &path);
    static bool TestForWrite(std::string const &path);
    static bool TestForExecute(std::string const &path);
    static bool TestForDirectory(std::string const &path);
    static bool TestForSymlink(std::string const &path);

public:
    static std::string GetDirectoryName(std::string const &path);
    static std::string GetBaseName(std::string const &path);
    static std::string GetBaseNameWithoutExtension(std::string const &path);
    static std::string GetRelativePath(std::string const &path, std::string const &to);
    static std::string GetFileExtension(std::string const &path);

public:
    static bool IsFileExtension(std::string const &path,
            std::string const &extension, bool insensitive = false);
    static bool IsFileExtension(std::string const &path,
            std::initializer_list <std::string> const &extensions,
            bool insensitive = false);

public:
    static bool IsAbsolutePath(std::string const &path);

public:
    static std::string ResolveRelativePath(std::string const &path, std::string const &workingDirectory);
    static std::string ResolvePath(std::string const &path);
    static std::string NormalizePath(std::string const &path);

public:
    static bool Touch(std::string const &path);
    static bool Remove(std::string const &path);
    static bool CreateDirectory(std::string const &path);

public:
    static bool EnumerateDirectory(std::string const &path,
            std::string const &pattern,
            std::function <bool(std::string const &)> const &cb,
            bool insensitive = false);

    inline static bool EnumerateDirectory(std::string const &path,
            std::function <bool(std::string const &)> const &cb,
            bool insensitive = false)
    { return EnumerateDirectory(path, std::string(), cb, insensitive); }

public:
    static bool EnumerateRecursive(std::string const &path,
            std::string const &pattern,
            std::function <bool(std::string const &)> const &cb,
            bool insensitive = false);

    inline static bool EnumerateRecursive(std::string const &path,
            std::function <bool(std::string const &)> const &cb,
            bool insensitive = false)
    { return EnumerateRecursive(path, std::string(), cb, insensitive); }

public:
    static std::string GetCurrentDirectory();

public:
    static std::string FindFile(std::string const &name, std::string const &paths);
    static std::string FindFile(std::string const &name, std::vector<std::string> const &paths);

public:
    static std::string FindExecutable(std::string const &name);
    static std::string FindExecutable(std::string const &name, std::string const &paths);
    static std::string FindExecutable(std::string const &name, std::vector<std::string> const &paths);

    template <typename... P>
    static inline std::string FindExecutables(P const &... names)
    {
        for (auto name : { names... }) {
            std::string path = FindExecutable(name);
            if (!path.empty())
                return path;
        }
        return std::string();
    }
};

}

#endif  // !__libutil_FSUtil_h
