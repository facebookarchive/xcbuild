// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/LegacyTarget.h>
#include <pbxproj/PBX/BuildPhases.h>

using pbxproj::PBX::LegacyTarget;

LegacyTarget::LegacyTarget() :
    Target                         (Isa(), kTypeLegacy),
    _passBuildSettingsInEnvironment(false)
{
}

bool LegacyTarget::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!Target::parse(context, dict))
        return false;

    auto BWD   = dict->value <plist::String> ("buildWorkingDirectory");
    auto BTP   = dict->value <plist::String> ("buildToolPath");
    auto BAS   = dict->value <plist::String> ("buildArgumentsString");
    auto PBSIE = dict->value <plist::Integer> ("passBuildSettingsInEnvironment");

    if (BWD != nullptr) {
        _buildWorkingDirectory = BWD->value();
    }

    if (BTP != nullptr) {
        _buildToolPath = BTP->value();
    }

    if (BAS != nullptr) {
        _buildArgumentsString = BAS->value();
    }

    if (PBSIE != nullptr) {
        _passBuildSettingsInEnvironment = (PBSIE->value() != 0);
    }

    return true;
}
