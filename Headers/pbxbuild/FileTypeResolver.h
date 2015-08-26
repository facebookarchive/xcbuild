// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_FileTypeResolver_h
#define __pbxbuild_FileTypeResolver_h

#include <pbxbuild/Base.h>
#include <pbxbuild/BuildEnvironment.h>

namespace pbxbuild {

class FileTypeResolver {
private:
    FileTypeResolver();
    ~FileTypeResolver();

public:
    static pbxspec::PBX::FileType::shared_ptr
    Resolve(pbxspec::Manager::shared_ptr const &specManager, std::string const &filePath);
    static pbxspec::PBX::FileType::shared_ptr
    Resolve(pbxspec::Manager::shared_ptr const &specManager, pbxproj::PBX::FileReference::shared_ptr const &fileReference, pbxsetting::Environment const &environment);
};

}

#endif // !__pbxbuild_DependencyResolver_h
