/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/Invocation.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <libutil/SysUtil.h>

namespace Tool = pbxbuild::Tool;
using AuxiliaryFile = pbxbuild::Tool::Invocation::AuxiliaryFile;
using DependencyInfo = pbxbuild::Tool::Invocation::DependencyInfo;
using Executable = pbxbuild::Tool::Invocation::Executable;
using libutil::Filesystem;
using libutil::FSUtil;
using libutil::SysUtil;

AuxiliaryFile::Chunk::
Chunk(Type type, ext::optional<std::vector<uint8_t>> const &data, ext::optional<std::string> const &file) :
    _type(type),
    _data(data),
    _file(file)
{
}

AuxiliaryFile::Chunk AuxiliaryFile::Chunk::
Data(std::vector<uint8_t> const &data)
{
    return Chunk(Type::Data, data, ext::nullopt);
}

AuxiliaryFile::Chunk AuxiliaryFile::Chunk::
File(std::string const &file)
{
    return Chunk(Type::File, ext::nullopt, file);
}

AuxiliaryFile::
AuxiliaryFile(std::string const &path, std::vector<Chunk> const &chunks, bool executable) :
    _path      (path),
    _chunks    (chunks),
    _executable(executable)
{
}

AuxiliaryFile AuxiliaryFile::
Data(std::string const &path, std::vector<uint8_t> const &data, bool executable)
{
    return AuxiliaryFile(path, { Chunk::Data(data) }, executable);
}

AuxiliaryFile AuxiliaryFile::
File(std::string const &path, std::string const &file, bool executable)
{
    return AuxiliaryFile(path, { Chunk::File(file) }, executable);
}

DependencyInfo::
DependencyInfo(dependency::DependencyInfoFormat format, std::string const &path) :
    _format(format),
    _path  (path)
{
}

Executable::
Executable(std::string const &path, std::string const &builtin) :
    _path   (path),
    _builtin(builtin)
{
}

std::string const &Executable::
displayName() const
{
    /* If the tool is builtin. */
    bool builtin = !_builtin.empty();

    /* The user-facing name of the executable. For builtin ones, prefer the shorter built-in name. */
    return (builtin ? _builtin : _path);
}

Executable Executable::
Determine(std::string const &executable, std::vector<std::string> const &executablePaths)
{
    std::string builtinPrefix = "builtin-";
    bool builtin = executable.compare(0, builtinPrefix.size(), builtinPrefix) == 0;

    if (builtin) {
        /* Has a builtin prefix. */
        return Builtin(executable);
    } else {
        std::string path = executable;

        if (!FSUtil::IsAbsolutePath(executable)) {
            /* Not absolute, look in the search paths. */
            // TODO(grp): Handle when the executable is not found.
            path = Filesystem::GetDefaultUNSAFE()->findExecutable(executable, executablePaths).value_or(std::string());
        }

        return Absolute(path);
    }
}

Executable Executable::
Absolute(std::string const &path)
{
    return Executable(path, std::string());
}

Executable Executable::
Builtin(std::string const &name)
{
    std::string executableRoot = FSUtil::GetDirectoryName(SysUtil::GetDefault()->executablePath());
    std::string path = executableRoot + "/" + name;
    return Executable(path, name);
}

Tool::Invocation::
Invocation() :
    _executable             (Executable(std::string(), std::string())),
    _showEnvironmentInLog   (true),
    _createsProductStructure(false)
{
}

Tool::Invocation::
~Invocation()
{
}

