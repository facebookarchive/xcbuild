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
#include <pbxbuild/TypeResolvedFile.h>

namespace pbxbuild {
namespace Phase {

class Environment;

/*
 * Represents a file to build as part of a phase.
 */
class File {
private:
    pbxproj::PBX::BuildFile::shared_ptr _buildFile;
    TypeResolvedFile                    _file;

private:
    std::string                         _outputSubdirectory;
    std::string                         _fileNameDisambiguator;

public:
    File(pbxproj::PBX::BuildFile::shared_ptr const &buildFile, TypeResolvedFile const &file, std::string const &outputSubdirectory, std::string const &fileNameDisambiguator);
    ~File();

public:
    pbxproj::PBX::BuildFile::shared_ptr const &buildFile() const
    { return _buildFile; }
    TypeResolvedFile const &file() const
    { return _file; }

public:
    std::string const &outputSubdirectory() const
    { return _outputSubdirectory; }
    std::string const &fileNameDisambiguator() const
    { return _fileNameDisambiguator; }

public:
    static std::unique_ptr<TypeResolvedFile>
    ResolveReferenceProxy(Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, pbxproj::PBX::ReferenceProxy::shared_ptr const &referenceProxy);
    static std::unique_ptr<TypeResolvedFile>
    ResolveFileReference(Phase::Environment const &phaseEnvironment, pbxsetting::Environment const &environment, pbxproj::PBX::FileReference::shared_ptr const &fileReference);

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
