// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/GroupItem.h>

using pbxproj::PBX::GroupItem;

GroupItem::GroupItem(std::string const &isa, Type type) :
    Object(isa),
    _type (type)
{
}

bool GroupItem::
parse(Context &context, plist::Dictionary const *dict)
{
    auto N  = dict->value <plist::String> ("name");
    auto ST = dict->value <plist::String> ("sourceTree");
    auto P  = dict->value <plist::String> ("path");

    if (N != nullptr) {
        _name = N->value();
    }

    if (ST != nullptr) {
        _sourceTree = ST->value();
    }

    if (P != nullptr) {
        _path = P->value();
    }

    return true;
}
