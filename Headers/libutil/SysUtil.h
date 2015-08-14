// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_SysUtil_h
#define __libutil_SysUtil_h

#include <libutil/Base.h>

namespace libutil {

class SysUtil {
public:
    static std::string GetUserName();
    static std::string GetGroupName();

    static int32_t GetUserID();
    static int32_t GetGroupID();

public:
    static void Sleep(uint64_t us, bool interruptible = false);
};

}

#endif  // !__libutil_SysUtil_h
