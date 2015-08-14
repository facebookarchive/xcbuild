// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_BaseGroup_h
#define __pbxproj_PBX_BaseGroup_h

#include <pbxproj/PBX/GroupItem.h>
#include <pbxproj/Context.h>

namespace pbxproj { namespace PBX {

class BaseGroup : public GroupItem {
public:
    typedef std::shared_ptr <BaseGroup> shared_ptr;

private:
    GroupItem::vector _children;

protected:
    BaseGroup(std::string const &isa, GroupItem::Type type);

public:
    inline GroupItem::vector const &children() const
    { return _children; }
    inline GroupItem::vector &children()
    { return _children; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_PBX_BaseGroup_h
