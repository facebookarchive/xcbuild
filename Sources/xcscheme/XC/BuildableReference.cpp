// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/BuildableReference.h>

using xcscheme::XC::BuildableReference;

BuildableReference::BuildableReference()
{
}

bool BuildableReference::
parse(plist::Dictionary const *dict)
{
    auto BlI = dict->value <plist::String> ("BlueprintIdentifier");
    auto BlN = dict->value <plist::String> ("BlueprintName");
    auto BuI = dict->value <plist::String> ("BuildableIdentifier");
    auto BuN = dict->value <plist::String> ("BuildableName");
    auto RC  = dict->value <plist::String> ("ReferencedContainer");

    if (BlI != nullptr) {
        _blueprintIdentifier = BlI->value();
    }

    if (BlN != nullptr) {
        _blueprintName = BlN->value();
    }

    if (BuI != nullptr) {
        _buildableIdentifier = BuI->value();
    }

    if (BuN != nullptr) {
        _buildableName = BuN->value();
    }

    if (RC != nullptr) {
        std::string location = RC->value();
        size_t      colon    = location.find(':');

        _referencedContainerType = location.substr(0, colon);
        _referencedContainer     = location.substr(colon + 1);
    }

    return true;
}
