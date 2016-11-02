/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/Tool/Input.h>
#include <pbxbuild/Target/Environment.h>

namespace Tool = pbxbuild::Tool;
namespace Target = pbxbuild::Target;

Tool::Input::
Input(
    pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
    Target::BuildRules::BuildRule::shared_ptr const &buildRule,
    pbxspec::PBX::FileType::shared_ptr const &fileType,
    std::string const &path,
    ext::optional<std::string> const &localization,
    ext::optional<std::string> const &fileNameDisambiguator) :
    _buildFile            (buildFile),
    _buildRule            (buildRule),
    _fileType             (fileType),
    _path                 (path),
    _localization         (localization),
    _fileNameDisambiguator(fileNameDisambiguator)
{
}

Tool::Input::
~Input()
{
}

