// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/Action.h>

using xcscheme::XC::Action;

Action::Action()
{
}

bool Action::
parse(plist::Dictionary const *dict)
{
    auto BC = dict->value <plist::String> ("buildConfiguration");

    if (BC != nullptr) {
        _buildConfiguration = BC->value();
    }

    return true;
}
