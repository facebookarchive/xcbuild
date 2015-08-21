// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/ActionContent.h>

using xcscheme::XC::ActionContent;

ActionContent::ActionContent()
{
}

bool ActionContent::
parse(plist::Dictionary const *dict)
{
    auto T   = dict->value <plist::String> ("title");
    auto ST  = dict->value <plist::String> ("scriptText");
    auto EB  = dict->value <plist::Dictionary> ("EnvironmentBuildable");
    auto ELR = dict->value <plist::String> ("emailRecipient");
    auto ELS = dict->value <plist::String> ("emailSubject");
    auto ELB = dict->value <plist::String> ("emailBody");
    auto ALE = dict->value <plist::Boolean> ("attachLogToEmail");

    if (T != nullptr) {
        _title = T->value();
    }

    if (ST != nullptr) {
        _scriptText = ST->value();
    }

    if (EB != nullptr) {
        auto BR = EB->value <plist::Dictionary> ("BuildableReference");
        if (BR != nullptr) {
            _environmentBuildable = std::make_shared <BuildableReference> ();
            if (!_environmentBuildable->parse(BR))
                return false;
        }
    }

    if (ELR != nullptr) {
        _emailRecipient = ELR->value();
    }

    if (ELS != nullptr) {
        _emailSubject = ELS->value();
    }

    if (ELB != nullptr) {
        _emailBody = ELB->value();
    }

    if (ALE != nullptr) {
        _attachLogToEmail = ALE->value();
    }

    return true;
}
