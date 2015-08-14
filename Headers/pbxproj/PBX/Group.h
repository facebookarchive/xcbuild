// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_Group_h
#define __pbxproj_PBX_Group_h

#include <pbxproj/PBX/BaseGroup.h>

namespace pbxproj { namespace PBX {

class Group : public BaseGroup {
public:
    typedef std::shared_ptr <Group> shared_ptr;

private:
    uint32_t _indentWidth;
    uint32_t _tabWidth;

public:
    Group();

public:
    inline uint32_t indentWidth() const
    { return _indentWidth; }

    inline uint32_t tabWidth() const
    { return _tabWidth; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXGroup; }
};

} }

#endif  // !__pbxproj_PBX_Group_h
