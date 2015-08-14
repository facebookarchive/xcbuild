// Copyright 2013-present Facebook. All Rights Reserved.

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

    auto IW = dict->value <plist::Integer> ("indentWidth");
    auto TW = dict->value <plist::Integer> ("tabWidth");

    if (IW != nullptr) {
        _indentWidth = IW->value();
    }

    if (TW != nullptr) {
        _tabWidth = TW->value();
    }

    return true;
}
