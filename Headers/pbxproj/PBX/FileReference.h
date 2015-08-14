// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_FileReference_h
#define __pbxproj_PBX_FileReference_h

#include <pbxproj/PBX/GroupItem.h>

namespace pbxproj { namespace PBX {

class FileReference : public GroupItem {
public:
    typedef std::shared_ptr <FileReference> shared_ptr;
    typedef std::vector <shared_ptr> vector;

private:
    std::string _lastKnownFileType;
    std::string _explicitFileType;
    bool        _includeInIndex;
    uint32_t    _fileEncoding;

public:
    FileReference();

public:
    inline std::string const &lastKnownFileType() const
    { return _lastKnownFileType; }

public:
    inline std::string const &explicitFileType() const
    { return _explicitFileType; }

public:
    inline bool includeInIndex() const
    { return _includeInIndex; }

public:
    inline uint32_t fileEncoding() const
    { return _fileEncoding; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXFileReference; }
};

} }

#endif  // !__pbxproj_PBX_FileReference_h
