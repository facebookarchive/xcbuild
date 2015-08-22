// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_GroupItem_h
#define __pbxproj_PBX_GroupItem_h

#include <pbxproj/PBX/Object.h>

namespace pbxproj { namespace PBX {

class GroupItem : public Object {
public:
    typedef std::shared_ptr <GroupItem> shared_ptr;
    typedef std::vector <shared_ptr> vector;

public:
    enum Type {
        kTypeGroup,
        kTypeVariantGroup,
        kTypeVersionGroup,
        kTypeFileReference,
        kTypeReferenceProxy
    };

private:
    Type        _type;

protected:
    friend class BaseGroup;
    GroupItem  *_parent;
    std::string _name;
    std::string _path;
    std::string _sourceTree;

protected:
    GroupItem(std::string const &isa, Type type);

public:
    inline Type type() const
    { return _type; }

public:
    inline std::string const &name() const
    { return _name.empty() ? _path : _name; }

public:
    inline std::string const &path() const
    { return _path; }

public:
    inline std::string const &sourceTree() const
    { return _sourceTree; }

public:
    pbxsetting::Value resolve(void) const;

protected:
    virtual bool parse(Context &context, plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_PBX_GroupItem_h
