// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_TypeResolvedFile_h
#define __pbxbuild_TypeResolvedFile_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>

namespace pbxbuild {

class TypeResolvedFile {
private:
    std::string                        _filePath;
    pbxspec::PBX::FileType::shared_ptr _fileType;

public:
    TypeResolvedFile(std::string const &filePath, pbxspec::PBX::FileType::shared_ptr const &fileType);
    ~TypeResolvedFile();

public:
    std::string const &filePath() const
    { return _filePath; }
    pbxspec::PBX::FileType::shared_ptr const &fileType() const
    { return _fileType; }

public:
    static std::unique_ptr<TypeResolvedFile>
    Resolve(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &domains, std::string const &filePath);
    static std::unique_ptr<TypeResolvedFile>
    Resolve(pbxspec::Manager::shared_ptr const &specManager, std::vector<std::string> const &domains, pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment);
};

}

#endif // !__pbxbuild_DependencyResolver_h
