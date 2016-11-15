/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/FSUtil.h>

#include <cstring>

#include <strings.h>

using libutil::FSUtil;

std::string FSUtil::
GetDirectoryName(std::string const &path)
{
    std::string::size_type pos = path.rfind('/');
    if (pos == std::string::npos) {
        return std::string();
    } else if (pos == 0) {
        return "/";
    } else {
        return path.substr(0, pos);
    }
}

std::string FSUtil::
GetBaseName(std::string const &path)
{
    std::string::size_type pos = path.rfind('/');
    if (pos == std::string::npos) {
        return path;
    } else {
        return path.substr(pos + 1);
    }
}

std::string FSUtil::
GetBaseNameWithoutExtension(std::string const &path)
{
    std::string base = GetBaseName(path);

    size_t pos = base.rfind('.');
    if (pos == std::string::npos) {
        return base;
    }

    return base.substr(0, pos);
}

std::string FSUtil::
GetRelativePath(std::string const &path, std::string const &to)
{
    std::string::size_type po = 0;
    std::string::size_type oo = 0;

    std::string result;

    bool found = false;
    while (!found) {
        std::string::size_type npo = path.find('/', po);
        std::string::size_type noo = to.find('/', oo);

        std::string spo = path.substr(po, (npo == std::string::npos ? path.size() : npo) - po);
        std::string soo = to.substr(oo, (noo == std::string::npos ? to.size() : noo) - oo);

        if (spo == soo) {
            po = (npo == std::string::npos ? std::string::npos : npo + 1);
            oo = (noo == std::string::npos ? std::string::npos : noo + 1);
        } else {
            break;
        }

        if (npo == std::string::npos || noo == std::string::npos) {
            break;
        }
    }

    while (oo != std::string::npos && oo != to.size()) {
        result += "../";
        oo = to.find('/', oo + 1);
    }

    if (po != std::string::npos && po != path.size()) {
        result += path.substr(po);
    }

    return result;
}

std::string FSUtil::
GetFileExtension(std::string const &path)
{
    std::string base = GetBaseName(path);

    size_t pos = base.rfind('.');
    if (pos == std::string::npos) {
        return std::string();
    }

    return base.substr(pos + 1);
}

bool FSUtil::
IsFileExtension(std::string const &path, std::string const &extension, bool insensitive)
{
    std::string pathExtension = GetFileExtension(path);
    if (pathExtension.empty()) {
        return extension.empty();
    }

    if (insensitive) {
        return ::strcasecmp(pathExtension.c_str(), extension.c_str()) == 0;
    } else {
        return pathExtension == extension;
    }
}

bool FSUtil::
IsFileExtension(std::string const &path, std::initializer_list<std::string> const &extensions, bool insensitive)
{
    std::string pathExtension = GetFileExtension(path);
    if (pathExtension.empty()) {
        return false;
    }

    for (auto const &extension : extensions) {
        bool match = false;
        if (insensitive) {
            match = ::strcasecmp(pathExtension.c_str(), extension.c_str()) == 0;
        } else {
            match = pathExtension == extension;
        }
        if (match)
            return true;
    }

    return false;
}

bool FSUtil::
IsAbsolutePath(std::string const &path)
{
    return !path.empty() && path[0] == '/';
}

std::string FSUtil::
ResolveRelativePath(std::string const &path, std::string const &workingDirectory)
{
    if (IsAbsolutePath(path)) {
        return path;
    } else if (path.empty()) {
        return workingDirectory;
    } else if (workingDirectory.empty()) {
        return path;
    } else {
        return NormalizePath(workingDirectory + "/" + path);
    }
}

static size_t
SimplePathNormalize(
    char const *in,
    char *out,
    size_t outSize,
    char separator,
    char const *invalidCharSet,
    bool dontWantRoot,
    bool relative,
    char replacementChar)
{
    char const *i = in;
    char *o = out;

    while (i[0] != 0) {
        if (i[0] == separator) {
            while (i[1] != 0 && i[1] == separator)
                i++;

            i++;
            if (o == out || *(o - 1) != separator) {
                if (o != out || !dontWantRoot)
                    *o++ = separator, *o = 0;
            }
        } else if (!relative && (i == in || i[-1] == separator) && i[0] == '.') {
            if (i[1] == '.' && (i[2] == separator || i[2] == 0)) {
                int cnt = 2;
                for (;;) {
                    if (cnt > 0 && *o == separator) {
                        if (o == out || --cnt == 0) {
                            o++;
                            break;
                        }
                    } else if (o == out) {
                        if (!dontWantRoot)
                            *o++ = separator, *o = 0;
                        break;
                    }
                    *o-- = 0;
                }

                i += (i[2] == 0 ? 2 : 3);
            } else if (i[1] == separator || i[1] == 0) {
                if (o == out) {
                    *o++ = '.', *o++ = separator;
                }
                *o = 0, i += (i[1] == 0 ? 1 : 2);
            } else {
                i++, *o++ = '.', *o = 0;
            }
        } else {
            if (invalidCharSet != NULL
                && strchr(invalidCharSet, *i) != NULL)
                *o++ = replacementChar, i++;
            else
                *o++ = *i++;
        }
    }

    *o = 0;

    return (o - out);
}

static size_t
POSIXPathNormalize(char const * in, char *out, size_t outSize, bool relative)
{
    return SimplePathNormalize(in, out, outSize, '/', NULL, false, relative, '-');
}

std::string FSUtil::
NormalizePath(std::string const &path)
{
    std::string outputPath;

    if (path.empty()) {
        return std::string();
    }

    outputPath.resize(path.size() * 2);
    size_t size = ::POSIXPathNormalize(path.c_str(), &outputPath[0], outputPath.size(), path[0] != '/');
    outputPath.resize(size);

    return outputPath;
}
