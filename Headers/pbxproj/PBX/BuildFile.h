// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_BuildFile_h
#define __pbxproj_PBX_BuildFile_h

#include <pbxproj/PBX/GroupItem.h>

namespace pbxproj { namespace PBX {

class BuildFile : public Object {
public:
    typedef std::shared_ptr <BuildFile> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    GroupItem::shared_ptr       _fileRef;
    std::vector<std::string>    _compilerFlags;
    std::vector<std::string>    _attributes;

public:
    BuildFile();
    ~BuildFile();

public:
    inline GroupItem::shared_ptr const &fileRef() const
    { return _fileRef; }
    inline GroupItem::shared_ptr &fileRef()
    { return _fileRef; }

public:
    inline std::vector<std::string> const &compilerFlags() const
    { return _compilerFlags; }
    inline std::vector<std::string> const &attributes() const
    { return _attributes; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXBuildFile; }
};

} }

#endif  // !__pbxproj_PBX_BuildFile_h
