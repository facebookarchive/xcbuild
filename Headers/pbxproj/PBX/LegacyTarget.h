// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxproj_PBX_LegacyTarget_h
#define __pbxproj_PBX_LegacyTarget_h

#include <pbxproj/PBX/Target.h>

namespace pbxproj { namespace PBX {

class LegacyTarget : public Target {
public:
    typedef std::shared_ptr <LegacyTarget> shared_ptr;

private:
    std::string _buildWorkingDirectory;
    std::string _buildToolPath;
    std::string _buildArgumentsString;
    bool        _passBuildSettingsInEnvironment;

public:
    LegacyTarget();

public:
    inline std::string const &buildToolPath() const
    { return _buildToolPath; }

public:
    inline std::string const &buildArgumentsString() const
    { return _buildArgumentsString; }

public:
    inline std::string const &buildWorkingDirectory() const
    { return _buildWorkingDirectory; }

public:
    inline bool passBuildSettingsInEnvironment() const
    { return _passBuildSettingsInEnvironment; }

public:
    bool parse(Context &context, plist::Dictionary const *dict);

public:
    static inline char const *Isa()
    { return ISA::PBXLegacyTarget; }
};

} }

#endif  // !__pbxproj_PBX_LegacyTarget_h
