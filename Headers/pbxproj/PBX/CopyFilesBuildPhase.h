// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_CopyFilesBuildPhase_h
#define __pbxproj_PBX_CopyFilesBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class CopyFilesBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <CopyFilesBuildPhase> shared_ptr;

public:
    enum Destination {
        kDestinationAbsolute         = 0,
        kDestinationWrapper          = 1,
        kDestinationExecutables      = 6,
        kDestinationResources        = 7,
        kDestinationPublicHeaders    = 8,
        kDestinationPrivateHeaders   = 9,
        kDestinationFrameworks       = 10,
        kDestinationSharedFrameworks = 11,
        kDestinationSharedSupport    = 12,
        kDestinationPlugIns          = 13,
        kDestinationScripts          = 14,
        kDestinationJavaResources    = 15,
        kDestinationProducts         = 16,
    };

private:
    std::string _dstPath;
    Destination _dstSubfolderSpec;

public:
    CopyFilesBuildPhase();

public:
    inline std::string const &dstPath() const
    { return _dstPath; }

public:
    inline Destination dstSubfolderSpec() const
    { return _dstSubfolderSpec; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXCopyFilesBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
