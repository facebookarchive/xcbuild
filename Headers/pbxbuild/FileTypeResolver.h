// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_FileTypeResolver_h
#define __pbxbuild_FileTypeResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>

namespace pbxbuild {

class FileTypeResolver {
private:
    std::string                        _filePath;
    pbxspec::PBX::FileType::shared_ptr _fileType;

public:
    FileTypeResolver(std::string const &filePath, pbxspec::PBX::FileType::shared_ptr const &fileType);
    ~FileTypeResolver();

public:
    std::string const &filePath() const
    { return _filePath; }
    pbxspec::PBX::FileType::shared_ptr const &fileType() const
    { return _fileType; }

public:
    static std::unique_ptr<FileTypeResolver>
    Resolve(pbxspec::Manager::shared_ptr const &specManager, std::string const &filePath);
    static std::unique_ptr<FileTypeResolver>
    Resolve(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment);
};

}

#endif // !__pbxbuild_DependencyResolver_h
