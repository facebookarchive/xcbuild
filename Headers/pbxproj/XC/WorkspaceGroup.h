// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_WorkspaceGroup_h
#define __pbxproj_XC_WorkspaceGroup_h

#include <pbxproj/XC/WorkspaceGroupItem.h>

namespace pbxproj { namespace XC {

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

#endif  // !__pbxproj_XC_WorkspaceGroup_h
