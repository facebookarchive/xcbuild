/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/XC/VersionGroup.h>
#include <pbxproj/PBX/FileReference.h>

using pbxproj::XC::VersionGroup;

VersionGroup::VersionGroup() :
    BaseGroup(Isa(), GroupItem::kTypeVersionGroup)
{
}

bool VersionGroup::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!BaseGroup::parse(context, dict))
        return false;

    std::string CVID;

    auto CV  = context.indirect <PBX::FileReference> (dict, "currentVersion", &CVID);
    auto VGT = dict->value <plist::String> ("versionGroupType");

    if (CV != nullptr) {
        _currentVersion = context.parseObject(context.fileReferences, CVID, CV);
        if (!_currentVersion)
            return false;
    }

    if (VGT != nullptr) {
        _versionGroupType = VGT->value();
    }

    return true;
}
