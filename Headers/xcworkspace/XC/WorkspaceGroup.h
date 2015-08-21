// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcworkspace_XC_WorkspaceGroup_h
#define __xcworkspace_XC_WorkspaceGroup_h

#include <xcworkspace/XC/WorkspaceGroupItem.h>

namespace xcworkspace { namespace XC {

class WorkspaceGroup : public WorkspaceGroupItem {
private:
    std::string                _name;
    WorkspaceGroupItem::vector _items;

public:
    WorkspaceGroup();

public:
    inline std::string const &name() const
    { return _name; }

public:
    inline WorkspaceGroupItem::vector const &items() const
    { return _items; }
    inline WorkspaceGroupItem::vector &items()
    { return _items; }

public:
    bool parse(plist::Dictionary const *dict) override;
};

} }

#endif  // !__xcworkspace_XC_WorkspaceGroup_h
