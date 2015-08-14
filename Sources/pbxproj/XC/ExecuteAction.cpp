// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/ExecuteAction.h>

using pbxproj::XC::ExecuteAction;

ExecuteAction::ExecuteAction()
{
}

bool ExecuteAction::
parse(plist::Dictionary const *dict)
{
    auto AT = dict->value <plist::String> ("ActionType");
    auto AC = dict->value <plist::Dictionary> ("ActionContent");

    if (AT != nullptr) {
        _actionType = AT->value();
    }

    if (AC != nullptr) {
        _actionContent = std::make_shared <ActionContent> ();
        if (!_actionContent->parse(AC))
            return false;
    }

    return true;
}

