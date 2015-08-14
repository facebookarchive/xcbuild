// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_WorkspaceGroupItem_h
#define __pbxproj_XC_WorkspaceGroupItem_h

#include <pbxproj/Base.h>

namespace pbxproj { namespace XC {

class WorkspaceGroupItem {
public:
    typedef std::shared_ptr <WorkspaceGroupItem> shared_ptr;
    typedef std::vector <shared_ptr> vector;

public:
    enum Type {
        kTypeGroup,
        kTypeFileRef
    };

private:
    Type        _type;
    std::string _locationType;
    std::string _location;

protected:
    WorkspaceGroupItem(Type type);

public:
    inline Type type() const
    { return _type; }

public:
    inline std::string const &locationType() const
    { return _locationType; }
    inline std::string const &location() const
    { return _location; }

public:
    virtual bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_XC_WorkspaceGroupItem_h
