// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_XC_Workspace_h
#define __pbxproj_XC_Workspace_h

#include <pbxproj/XC/WorkspaceGroupItem.h>
#include <pbxproj/XC/Scheme.h>

namespace pbxproj { namespace XC {

class Workspace {
public:
    typedef std::shared_ptr <Workspace> shared_ptr;

private:
    std::string                _projectFile;
    std::string                _basePath;
    std::string                _name;

private:
    WorkspaceGroupItem::vector _items;
    Scheme::vector             _schemes;
    Scheme::shared_ptr         _defaultScheme;

public:
    Workspace();

public:
    static shared_ptr Open(std::string const &path);

public:
    inline std::string const &projectFile() const
    { return _projectFile; }
    inline std::string const &basePath() const
    { return _basePath; }
    inline std::string const &name() const
    { return _name; }

public:
    inline WorkspaceGroupItem::vector const &items() const
    { return _items; }
    inline WorkspaceGroupItem::vector &items()
    { return _items; }

public:
    inline Scheme::vector const &schemes() const
    { return _schemes; }
    inline Scheme::vector &schemes()
    { return _schemes; }

public:
    inline XC::Scheme::shared_ptr const &defaultScheme() const
    { return _defaultScheme; }
    inline XC::Scheme::shared_ptr &defaultScheme()
    { return _defaultScheme; }

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__pbxproj_XC_Workspace_h
