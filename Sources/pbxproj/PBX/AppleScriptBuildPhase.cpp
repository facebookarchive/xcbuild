// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/AppleScriptBuildPhase.h>

using pbxproj::PBX::AppleScriptBuildPhase;

AppleScriptBuildPhase::AppleScriptBuildPhase() :
    BuildPhase(Isa(), kTypeAppleScript)
{
}

bool AppleScriptBuildPhase::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!BuildPhase::parse(context, dict))
        return false;

    auto CN = dict->value <plist::String> ("contextName");
    auto SC = dict->value <plist::Integer> ("isSharedContext");

    if (CN != nullptr) {
        _contextName = CN->value();
    }

    if (SC != nullptr) {
        _isSharedContext = (SC->value() != 0);
    }

    return true;
}
