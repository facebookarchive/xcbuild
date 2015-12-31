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

namespace pbxbuild {
namespace Phase {

class Environment;

/*
 * Represents a file to build as part of a phase.
 */
class File {
private:
    pbxproj::PBX::BuildFile::shared_ptr _buildFile;
    pbxspec::PBX::FileType::shared_ptr  _fileType;

private:
    std::string                         _path;
    std::string                         _outputSubdirectory;
    std::string                         _fileNameDisambiguator;

public:
    File(pbxproj::PBX::BuildFile::shared_ptr const &buildFile, pbxspec::PBX::FileType::shared_ptr const &fileType, std::string const &path, std::string const &outputSubdirectory, std::string const &fileNameDisambiguator);
    ~File();

public:
    pbxproj::PBX::BuildFile::shared_ptr const &buildFile() const
    { return _buildFile; }
    pbxspec::PBX::FileType::shared_ptr const &fileType() const
    { return _fileType; }

public:
    std::string const &path() const
    { return _path; }
    std::string const &outputSubdirectory() const
    { return _outputSubdirectory; }
    std::string const &fileNameDisambiguator() const
    { return _fileNameDisambiguator; }

public:
    static std::unique_ptr<Phase::File> ResolveReferenceProxy(
        Phase::Environment const &phaseEnvironment,
        pbxsetting::Environment const &environment,
        pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
        pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy);
    static Phase::File ResolveFileReference(
        Phase::Environment const &phaseEnvironment,
        pbxsetting::Environment const &environment,
        pbxproj::PBX::BuildFile::shared_ptr const &buildFile,
        pbxproj::PBX::FileReference::shared_ptr const &fileReference,
        std::string const &outputSubdirectory = std::string(),
        std::string const &fileNameDisambiguator = std::string());

public:
    /*
     * Resolves build files into `Phase::File`s, using the phase environment. Note that due to
     * certain types of build files being more complex than a single `Phase::File`, the number
     * of `Phase::File`s returned may be different than the number of build files passed in.
     */
    static std::vector<Phase::File>
    ResolveBuildFiles(Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, std::vector<pbxproj::PBX::BuildFile::shared_ptr> const &buildFiles);
};

}
}

#endif // !__pbxbuild_Phase_File_h
