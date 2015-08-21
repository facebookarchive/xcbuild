// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __xcworkspace_XC_Workspace_h
#define __xcworkspace_XC_Workspace_h

#include <xcscheme/XC/Scheme.h>
#include <xcworkspace/XC/WorkspaceGroupItem.h>

namespace xcworkspace { namespace XC {

class Workspace {
public:
    typedef std::shared_ptr <Workspace> shared_ptr;

private:
    std::string                _projectFile;
    std::string                _basePath;
    std::string                _name;

private:
    WorkspaceGroupItem::vector       _items;
    xcscheme::XC::Scheme::vector     _schemes;
    xcscheme::XC::Scheme::shared_ptr _defaultScheme;

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
    inline xcscheme::XC::Scheme::vector const &schemes() const
    { return _schemes; }
    inline xcscheme::XC::Scheme::vector &schemes()
    { return _schemes; }

public:
    inline xcscheme::XC::Scheme::shared_ptr const &defaultScheme() const
    { return _defaultScheme; }
    inline xcscheme::XC::Scheme::shared_ptr &defaultScheme()
    { return _defaultScheme; }

private:
    bool parse(plist::Dictionary const *dict);
};

} }

#endif  // !__xcworkspace_XC_Workspace_h
