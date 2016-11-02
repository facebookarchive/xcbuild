/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Phase_File_h
#define __pbxbuild_Phase_File_h

#include <pbxbuild/Base.h>
#include <pbxbuild/Target/BuildRules.h>

#include <string>
#include <ext/optional>

namespace libutil { class Filesystem; }
namespace pbxsetting { class Environment; }

namespace pbxbuild {
namespace Phase {

class Environment;

/*
 * Represents a resolved file to build as part of a phase.
 */
class File {
private:
    pbxproj::PBX::BuildFile::shared_ptr       _buildFile;
    Target::BuildRules::BuildRule::shared_ptr _buildRule;
    pbxspec::PBX::FileType::shared_ptr        _fileType;

private:
    std::string                               _path;
    ext::optional<std::string>                _localization;
    ext::optional<std::string>                _fileNameDisambiguator;

public:
    File(
        pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
        Target::BuildRules::BuildRule::shared_ptr const &buildRule,
        pbxspec::PBX::FileType::shared_ptr const &fileType,
        std::string const &path,
        ext::optional<std::string> const &localization,
        ext::optional<std::string> const &fileNameDisambiguator);
    ~File();

public:
    /*
     * The build file entry saying *to* build this file.
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
     * The localization this file is for.  This is relevant for variant groups,
     * which contain many versions of the same file for different lproj outputs.
     */
    ext::optional<std::string> const &localization() const
    { return _localization; }

    /*
     * A disambiguation identifier for files that have multiple with the
     * same base name within a target. This should be used for the output
     * path in order to avoid overwriting outputs.
     */
    ext::optional<std::string> const &fileNameDisambiguator() const
    { return _fileNameDisambiguator; }

public:
    /*
     * Resolves build files into `Phase::File`s, using the phase environment. Note that due to
     * certain types of build files being more complex than a single `Phase::File`, the number
     * of `Phase::File`s returned may be different than the number of build files passed in.
     */
    static std::vector<Phase::File>
    ResolveBuildFiles(libutil::Filesystem const *filesystem, Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, std::vector<pbxproj::PBX::BuildFile::shared_ptr> const &buildFiles);
};

}
}

#endif // !__pbxbuild_Phase_File_h
