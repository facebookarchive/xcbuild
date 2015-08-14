// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_ResourcesBuildPhase_h
#define __pbxproj_PBX_ResourcesBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class ResourcesBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <ResourcesBuildPhase> shared_ptr;

public:
    ResourcesBuildPhase();

public:
    static inline char const *Isa()
    { return ISA::PBXResourcesBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
