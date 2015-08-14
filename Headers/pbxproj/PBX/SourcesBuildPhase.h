// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_SourcesBuildPhase_h
#define __pbxproj_PBX_SourcesBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class SourcesBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <SourcesBuildPhase> shared_ptr;

public:
    SourcesBuildPhase();

public:
    static inline char const *Isa()
    { return ISA::PBXSourcesBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
