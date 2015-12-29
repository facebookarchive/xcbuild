/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/Invocation.h>
#include <pbxbuild/Build/Context.h>

namespace Tool = pbxbuild::Tool;
using AuxiliaryFile = pbxbuild::Tool::Invocation::AuxiliaryFile;

AuxiliaryFile::
AuxiliaryFile(std::string const &path, std::vector<char> const &contents, bool executable) :
    _path      (path),
    _contents  (contents),
    _executable(executable)
{
}

AuxiliaryFile::
AuxiliaryFile(std::string const &path, std::string const &contents, bool executable) :
    _path      (path),
    _contents  (std::vector<char>(contents.begin(), contents.end())),
    _executable(executable)
{
}

AuxiliaryFile::
~AuxiliaryFile()
{
}

Tool::Invocation::
Invocation() :
    _showEnvironmentInLog   (true),
    _createsProductStructure(false)
{
}

Tool::Invocation::
~Invocation()
{
}

