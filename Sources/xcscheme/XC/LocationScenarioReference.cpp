// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/LocationScenarioReference.h>

using xcscheme::XC::LocationScenarioReference;

LocationScenarioReference::LocationScenarioReference() :
    _referenceType(0)
{
}

bool LocationScenarioReference::
parse(plist::Dictionary const *dict)
{
    auto I  = dict->value <plist::String> ("identifier");
    auto RT = dict->value <plist::String> ("referenceType");

    if (I != nullptr) {
        _identifier = I->value();
    }

    if (RT != nullptr) {
        _referenceType = std::stoi(RT->value());
    }

    return true;
}
