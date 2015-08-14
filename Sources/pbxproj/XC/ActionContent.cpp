// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/ActionContent.h>

using pbxproj::XC::ActionContent;

ActionContent::ActionContent()
{
}

bool ActionContent::
parse(plist::Dictionary const *dict)
{
    auto T  = dict->value <plist::String> ("title");
    auto ST = dict->value <plist::String> ("scriptText");
    auto EB = dict->value <plist::Dictionary> ("EnvironmentBuildable");

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

    return true;
}
