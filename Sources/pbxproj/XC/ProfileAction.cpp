// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/ProfileAction.h>

using pbxproj::XC::ProfileAction;

ProfileAction::ProfileAction() :
    _debugDocumentVersioning     (false),
    _shouldUseLaunchSchemeArgsEnv(false),
    _useCustomWorkingDirectory   (false)
{
}

bool ProfileAction::
parse(plist::Dictionary const *dict)
{
    if (!Action::parse(dict))
        return false;
    
    auto DDV    = dict->value <plist::Boolean> ("debugDocumentVersioning");
    auto SULSAE = dict->value <plist::Boolean> ("shouldUseLaunchSchemeArgsEnv");
    auto UCWD   = dict->value <plist::Boolean> ("useCustomWorkingDirectory");
    auto STI    = dict->value <plist::String> ("savedToolIdentifier");
    auto BPR    = dict->value <plist::Dictionary> ("BuildableProductRunnable");

    if (DDV != nullptr) {
        _debugDocumentVersioning = DDV->value();
    }

    if (SULSAE != nullptr) {
        _shouldUseLaunchSchemeArgsEnv = SULSAE->value();
    }

    if (UCWD != nullptr) {
        _useCustomWorkingDirectory = UCWD->value();
    }

    if (STI != nullptr) {
        _savedToolIdentifier = STI->value();
    }

    if (BPR != nullptr) {
        auto BR = BPR->value <plist::Dictionary> ("BuildableReference");
        if (BR != nullptr) {
            _buildableProductRunnable = std::make_shared <BuildableReference> ();
            if (!_buildableProductRunnable->parse(BR))
                return false;
        }
    }

    return true;
}
