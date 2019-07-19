/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <pbxbuild/Tool/ModuleMapInfo.h>

namespace Tool = pbxbuild::Tool;

Tool::ModuleMapInfo::Entry::
Entry(
    Tool::AuxiliaryFile::Chunk const &contents,
    std::string const &intermediatePath,
    std::string const &finalPath) :
    _contents        (contents),
    _intermediatePath(intermediatePath),
    _finalPath       (finalPath)
{
}

Tool::ModuleMapInfo::
ModuleMapInfo()
{
}

