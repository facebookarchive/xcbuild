// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxsdk_SDK_Manager_h
#define __pbxsdk_SDK_Manager_h

#include <pbxsdk/SDK/Platform.h>

namespace pbxsdk { namespace SDK {

class Manager {
public:
    static bool GetPlatforms(std::string const &path, Platform::vector &platforms);
};

} }

#endif  // !__pbxsdk_SDK_Manager_h
