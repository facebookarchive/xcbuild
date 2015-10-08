// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcworkspace_XC_Group_h
#define __xcworkspace_XC_Group_h

#include <xcworkspace/XC/GroupItem.h>

namespace xcworkspace { namespace XC {

class Group : public GroupItem {
public:
    typedef std::shared_ptr <Group> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string       _name;
    GroupItem::vector _items;

public:
    Group();

public:
    inline std::string const &name() const
    { return _name; }

public:
    inline GroupItem::vector const &items() const
    { return _items; }
    inline GroupItem::vector &items()
    { return _items; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__xcworkspace_XC_Group_h
