// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_CopyFilesBuildPhase_h
#define __pbxproj_PBX_CopyFilesBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class CopyFilesBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <CopyFilesBuildPhase> shared_ptr;

private:
    std::string _dstPath;
    uint32_t    _dstSubfolderSpec;

public:
    CopyFilesBuildPhase();

public:
    inline std::string const &dstPath() const
    { return _dstPath; }

public:
    inline uint32_t dstSubfolderSpec() const
    { return _dstSubfolderSpec; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXCopyFilesBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
