/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxsetting_DefaultSettings_h
#define __pbxsetting_DefaultSettings_h

#include <pbxsetting/Base.h>
#include <pbxsetting/Level.h>
#include <pbxsetting/Setting.h>

namespace pbxsetting {

class DefaultSettings {
private:
    DefaultSettings();
    ~DefaultSettings();

public:
    static Level
    Environment(void);
    static Level
    Internal(void);
    static Level
    Local(void);
    static Level
    System(void);
    static Level
    Architecture(void);
    static Level
    Build(void);

public:
    static std::vector<Level>
    Levels(void);
};

}

#endif  // !__pbxsetting_Default_h
