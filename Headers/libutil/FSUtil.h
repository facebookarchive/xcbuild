// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_FSUtil_h
#define __libutil_FSUtil_h

#include <libutil/Base.h>

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
    static std::string ResolvePath(std::string const &path);
    static std::string NormalizePath(std::string const &path);

public:
    static bool Touch(std::string const &path);
    static bool Remove(std::string const &path);

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
    static libutil::string_vector GetDirectoryContents(std::string const &path,
            std::string const &pattern, bool insensitive = false);
    static libutil::string_vector GetDirectoryContents(std::string const &path,
            std::initializer_list <std::string> const &patterns,
            bool insensitive = false);

public:
    static std::string GetCurrentDirectory();

public:
    static std::string FindFile(std::string const &name, std::string const &paths);
    static std::string FindFile(std::string const &name, string_vector const &paths);

public:
    static std::string FindExecutable(std::string const &name);
    static std::string FindExecutable(std::string const &name, std::string const &paths);
    static std::string FindExecutable(std::string const &name, string_vector const &paths);

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
