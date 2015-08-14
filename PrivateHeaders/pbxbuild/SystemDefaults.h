// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_SystemDefaults_h
#define __pbxbuild_SystemDefaults_h

#include <pbxbuild/Base.h>

namespace pbxbuild {

//
// TODO This should be the SDK!
//
class SystemDefaults {
public:
    static std::string GetUsrPath();
    static std::string GetBinPath();
    static std::string GetCompilerPath();
};

}

#endif  // !__pbxbuild_SystemDefaults_h
