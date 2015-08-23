// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcworkspace/XC/GroupItem.h>

using xcworkspace::XC::GroupItem;

GroupItem::GroupItem(Type type) :
    _type(type)
{
}

bool GroupItem::
parse(plist::Dictionary const *dict)
{
    auto L = dict->value <plist::String> ("location");

    if (L != nullptr) {
        std::string location = L->value();
        size_t colon = location.find(':');
        if (colon != std::string::npos) {
            _location     = location.substr(colon + 1);
            _locationType = location.substr(0, colon);
        }
    }

    return true;
}
