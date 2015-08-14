// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_VariantGroup_h
#define __pbxproj_PBX_VariantGroup_h

#include <pbxproj/PBX/BaseGroup.h>

namespace pbxproj { namespace PBX {

class VariantGroup : public BaseGroup {
public:
    typedef std::shared_ptr <VariantGroup> shared_ptr;

public:
    VariantGroup();

public:
    static inline char const *Isa()
    { return ISA::PBXVariantGroup; }
};

} }

#endif  // !__pbxproj_PBX_VariantGroup_h
