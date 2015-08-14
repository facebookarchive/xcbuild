// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_SDK_Manager_h
#define __pbxproj_SDK_Manager_h

#include <pbxproj/SDK/Platform.h>

namespace pbxproj { namespace SDK {

class Manager {
public:
    static bool GetPlatforms(std::string const &path, Platform::vector &platforms);
};

} }

#endif  // !__pbxproj_SDK_Manager_h
