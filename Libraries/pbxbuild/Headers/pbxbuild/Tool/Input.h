/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Tool_Input_h
#define __pbxbuild_Tool_Input_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Target/BuildRules.h>

#include <string>
#include <ext/optional>

namespace libutil { class Filesystem; }
namespace pbxsetting { class Environment; }

namespace pbxbuild {
namespace Tool {

class Environment;

/*
 * Represents a resolved file to build as part of a phase.
 */
class Input {
private:
    pbxproj::PBX::BuildFile::shared_ptr       _buildFile;
    Target::BuildRules::BuildRule::shared_ptr _buildRule;
    pbxspec::PBX::FileType::shared_ptr        _fileType;

private:
    std::string                               _path;
    ext::optional<std::string>                _localization;
    ext::optional<std::string>                _fileNameDisambiguator;

public:
    Input(
        pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
        Target::BuildRules::BuildRule::shared_ptr const &buildRule,
        pbxspec::PBX::FileType::shared_ptr const &fileType,
        std::string const &path,
        ext::optional<std::string> const &localization,
        ext::optional<std::string> const &fileNameDisambiguator);
    ~Input();

public:
    /*
     * The build file entry saying *to* build this file. May be nullptr.
     */
    pbxproj::PBX::BuildFile::shared_ptr const &buildFile() const
    { return _buildFile; }

    /*
     * The build file entry saying *how* to build this file.
     */
    Target::BuildRules::BuildRule::shared_ptr const &buildRule() const
    { return _buildRule; }

    /*
     * The type of this file.
     */
    pbxspec::PBX::FileType::shared_ptr const &fileType() const
    { return _fileType; }

public:
    /*
     * The resolved absolute path to the file.
     */
    std::string const &path() const
    { return _path; }

    /*
     * The localization this file is for. This is relevant for variant groups,
     * which contain many versions of the same file for different lproj outputs.
     */
    ext::optional<std::string> const &localization() const
    { return _localization; }

    /*
     * A disambiguation identifier for different files with the same base name
     * within a target. This should be used for the output path along with the
     * base name in order to avoid overwriting outputs.
     */
    ext::optional<std::string> const &fileNameDisambiguator() const
    { return _fileNameDisambiguator; }
};

}
}

#endif // !__pbxbuild_Tool_Input_h
