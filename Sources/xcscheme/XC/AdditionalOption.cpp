// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcscheme/XC/AdditionalOption.h>

using xcscheme::XC::AdditionalOption;

AdditionalOption::AdditionalOption() :
    _isEnabled(false)
{
}

bool AdditionalOption::
parse(plist::Dictionary const *dict)
{
    auto IE = dict->value <plist::Boolean> ("isEnabled");
    auto K  = dict->value <plist::String> ("key");

    if (IE != nullptr) {
        _isEnabled = IE->value();
    }

    if (K != nullptr) {
        _key = K->value();
    }

    if (auto V = dict->value <plist::String> ("value")) {
        _value = V->value();
    } else if (auto V = dict->value <plist::Boolean> ("value")) {
        _value = V->value() ? "YES" : "NO";
    }

    return true;
}
