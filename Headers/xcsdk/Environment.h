// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcsdk_Environment_h
#define __xcsdk_Environment_h

#include <xcsdk/Base.h>

namespace xcsdk {

class Environment {
private:
    Environment();
    ~Environment();

public:
    static std::string DeveloperRoot(void);
};

}

#endif // !__xcsdk_Environment_h
