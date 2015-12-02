/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/LegacyTarget.h>
#include <pbxproj/PBX/BuildPhases.h>

using pbxproj::PBX::LegacyTarget;

LegacyTarget::LegacyTarget() :
    Target                         (Isa(), kTypeLegacy),
    _buildArgumentsString          (pbxsetting::Value::Empty()),
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
    auto PBSIE = dict->value <plist::String> ("passBuildSettingsInEnvironment");

    if (BWD != nullptr) {
        _buildWorkingDirectory = BWD->value();
    }

    if (BTP != nullptr) {
        _buildToolPath = BTP->value();
    }

    if (BAS != nullptr) {
        _buildArgumentsString = pbxsetting::Value::Parse(BAS->value());
    }

    if (PBSIE != nullptr) {
        _passBuildSettingsInEnvironment = (pbxsetting::Type::ParseInteger(PBSIE->value()) != 0);
    }

    return true;
}
