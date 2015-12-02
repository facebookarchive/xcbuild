/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
    auto SC = dict->value <plist::String> ("isSharedContext");

    if (CN != nullptr) {
        _contextName = CN->value();
    }

    if (SC != nullptr) {
        _isSharedContext = (pbxsetting::Type::ParseInteger(SC->value()) != 0);
    }

    return true;
}
