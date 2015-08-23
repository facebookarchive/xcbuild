// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcworkspace_XC_GroupItem_h
#define __xcworkspace_XC_GroupItem_h

#include <xcworkspace/Base.h>

namespace xcworkspace { namespace XC {

class Workspace;

class GroupItem {
public:
    typedef std::shared_ptr <GroupItem> shared_ptr;
    typedef std::vector <shared_ptr> vector;

public:
    enum Type {
        kTypeGroup,
        kTypeFileRef
    };

private:
    friend class Group;
    friend class Workspace;
    GroupItem  *_parent;
    Type        _type;
    std::string _locationType;
    std::string _location;

protected:
    GroupItem(Type type);

public:
    inline Type type() const
    { return _type; }

public:
    std::string resolve(std::shared_ptr<Workspace> const &workspace) const;

public:
    inline std::string const &locationType() const
    { return _locationType; }
    inline std::string const &location() const
    { return _location; }

public:
    virtual bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcworkspace_XC_GroupItem_h
