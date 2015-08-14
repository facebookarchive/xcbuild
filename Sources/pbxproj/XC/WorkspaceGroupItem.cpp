// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/WorkspaceGroupItem.h>

using pbxproj::XC::WorkspaceGroupItem;

WorkspaceGroupItem::WorkspaceGroupItem(Type type) :
    _type(type)
{
}

bool WorkspaceGroupItem::
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
