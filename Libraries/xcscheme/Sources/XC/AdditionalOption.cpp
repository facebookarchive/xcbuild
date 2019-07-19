/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <xcscheme/XC/AdditionalOption.h>
#include <plist/Boolean.h>
#include <plist/Dictionary.h>
#include <plist/String.h>

using xcscheme::XC::AdditionalOption;

AdditionalOption::
AdditionalOption() :
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
