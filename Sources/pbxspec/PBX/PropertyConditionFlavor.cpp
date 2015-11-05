/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/PropertyConditionFlavor.h>

using pbxspec::PBX::PropertyConditionFlavor;

PropertyConditionFlavor::PropertyConditionFlavor() :
    Specification(),
    _precedence  (0)
{
}

PropertyConditionFlavor::~PropertyConditionFlavor()
{
}

PropertyConditionFlavor::shared_ptr PropertyConditionFlavor::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    PropertyConditionFlavor::shared_ptr result;
    result.reset(new PropertyConditionFlavor());

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool PropertyConditionFlavor::
parse(Context *context, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "PropertyConditionFlavor",
        // Specification
        plist::MakeKey <plist::String> ("Class"),
        plist::MakeKey <plist::String> ("Type"),
        plist::MakeKey <plist::String> ("Identifier"),
        plist::MakeKey <plist::String> ("BasedOn"),
        plist::MakeKey <plist::String> ("Domain"),
        plist::MakeKey <plist::Boolean> ("IsGlobalDomainInUI"),
        plist::MakeKey <plist::String> ("Name"),
        plist::MakeKey <plist::String> ("Description"),
        plist::MakeKey <plist::String> ("Vendor"),
        plist::MakeKey <plist::String> ("Version"),
        // PropertyConditionFlavor
        plist::MakeKey <plist::Integer> ("Precedence"));

    if (!Specification::parse(context, dict))
        return false;

    auto P = dict->value <plist::Integer> ("Precedence");

    if (P != nullptr) {
        _precedence = P->value();
    }

    return true;
}

bool PropertyConditionFlavor::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != PropertyConditionFlavor::Type())
        return false;

    return inherit(reinterpret_cast <PropertyConditionFlavor::shared_ptr const &> (base));
}

bool PropertyConditionFlavor::
inherit(PropertyConditionFlavor::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _precedence = base->precedence();

    return true;
}
