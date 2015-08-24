// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxspec_Context_h
#define __pbxspec_Context_h

#include <pbxspec/Manager.h>

namespace pbxspec {

struct Context {
public:
    Manager *manager;
    std::string domain;
};

}

#endif  // !__pbxspec_Context_h
