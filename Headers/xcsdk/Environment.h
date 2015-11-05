/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
