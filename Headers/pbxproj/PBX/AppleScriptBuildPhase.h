// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_AppleScriptBuildPhase_h
#define __pbxproj_PBX_AppleScriptBuildPhase_h

#include <pbxproj/PBX/BuildPhase.h>

namespace pbxproj { namespace PBX {

class AppleScriptBuildPhase : public BuildPhase {
public:
    typedef std::shared_ptr <AppleScriptBuildPhase> shared_ptr;

private:
    std::string _contextName;
    bool        _isSharedContext;

public:
    AppleScriptBuildPhase();

public:
    inline std::string const &contextName() const
    { return _contextName; }

public:
    inline bool isSharedContext() const
    { return _isSharedContext; }

public:
    virtual bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXAppleScriptBuildPhase; }
};

} }

#endif  // !__pbxproj_PBX_BuildPhase_h
