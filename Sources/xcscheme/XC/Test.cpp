// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/Test.h>

using xcscheme::XC::Test;

Test::Test()
{
}

bool Test::
parse(plist::Dictionary const *dict)
{
    auto I = dict->value <plist::String> ("Identifier");

    if (I != nullptr) {
        _identifier = I->value();
    }

    return true;
}
