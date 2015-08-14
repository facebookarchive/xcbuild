// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_BuildFile_h
#define __pbxproj_PBX_BuildFile_h

#include <pbxproj/PBX/FileReference.h>

namespace pbxproj { namespace PBX {

class BuildFile : public Object {
public:
    typedef std::shared_ptr <BuildFile> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    FileReference::shared_ptr  _fileRef;
    plist::Object             *_settings;

public:
    BuildFile();
    ~BuildFile();

public:
    inline FileReference::shared_ptr const &fileRef() const
    { return _fileRef; }
    inline FileReference::shared_ptr &fileRef()
    { return _fileRef; }

public:
    inline plist::Object const *settings() const
    { return _settings; }
    inline plist::Object *settings()
    { return _settings; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXBuildFile; }
};

} }

#endif  // !__pbxproj_PBX_BuildFile_h
