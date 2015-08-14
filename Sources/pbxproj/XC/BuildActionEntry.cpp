// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/BuildActionEntry.h>

using pbxproj::XC::BuildActionEntry;

BuildActionEntry::BuildActionEntry() :
    _buildForAnalyzing(false),
    _buildForArchiving(false),
    _buildForProfiling(false),
    _buildForRunning  (false),
    _buildForTesting  (false)
{
}

bool BuildActionEntry::
parse(plist::Dictionary const *dict)
{
    auto BFAn = dict->value <plist::Boolean> ("buildForAnalyzing");
    auto BFAr = dict->value <plist::Boolean> ("buildForArchiving");
    auto BFP  = dict->value <plist::Boolean> ("buildForProfiling");
    auto BFR  = dict->value <plist::Boolean> ("buildForRunning");
    auto BFT  = dict->value <plist::Boolean> ("buildForTesting");
    auto BR   = dict->value <plist::Dictionary> ("BuildableReference");

    if (BFAn != nullptr) {
        _buildForAnalyzing = BFAn->value();
    }

    if (BFAr != nullptr) {
        _buildForArchiving = BFAr->value();
    }

    if (BFP != nullptr) {
        _buildForProfiling = BFP->value();
    }

    if (BFR != nullptr) {
        _buildForRunning = BFR->value();
    }

    if (BFT != nullptr) {
        _buildForTesting = BFT->value();
    }

    if (BR != nullptr) {
        _buildableReference = std::make_shared <BuildableReference> ();
        if (!_buildableReference->parse(BR))
            return false;
    }

    return true;
}
