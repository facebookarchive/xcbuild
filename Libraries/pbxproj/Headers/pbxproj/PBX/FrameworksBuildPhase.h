/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#ifndef __pbxproj_PBX_FrameworksBuildPhase_h
#define __pbxproj_PBX_FrameworksBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class FrameworksBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <FrameworksBuildPhase> shared_ptr;

public:
    FrameworksBuildPhase();

public:
    static inline char const *Isa()
    { return ISA::PBXFrameworksBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
