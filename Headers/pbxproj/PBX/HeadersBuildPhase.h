// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_HeadersBuildPhase_h
#define __pbxproj_PBX_HeadersBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class HeadersBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <HeadersBuildPhase> shared_ptr;

public:
    HeadersBuildPhase();

public:
    static inline char const *Isa()
    { return ISA::PBXHeadersBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
