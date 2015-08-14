// Copyright 2013-present Facebook. All Rights Reserved.

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
