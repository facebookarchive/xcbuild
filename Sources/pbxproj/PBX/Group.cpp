/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/Group.h>

using pbxproj::PBX::Group;

Group::Group() :
    BaseGroup   (Isa(), GroupItem::kTypeGroup),
    _indentWidth(0),
    _tabWidth   (0)
{
}

bool Group::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!BaseGroup::parse(context, dict))
        return false;

    auto IW = dict->value <plist::String> ("indentWidth");
    auto TW = dict->value <plist::String> ("tabWidth");

    if (IW != nullptr) {
        _indentWidth = pbxsetting::Type::ParseInteger(IW->value());
    }

    if (TW != nullptr) {
        _tabWidth = pbxsetting::Type::ParseInteger(TW->value());
    }

    return true;
}
