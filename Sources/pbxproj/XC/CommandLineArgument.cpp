// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/CommandLineArgument.h>

using pbxproj::XC::CommandLineArgument;

CommandLineArgument::CommandLineArgument() :
    _isEnabled(false)
{
}

bool CommandLineArgument::
parse(plist::Dictionary const *dict)
{
    auto IE = dict->value <plist::Boolean> ("isEnabled");
    auto A  = dict->value <plist::String> ("argument");

    if (IE != nullptr) {
        _isEnabled = IE->value();
    }

    if (A != nullptr) {
        _argument = A->value();
    }

    return true;
}
