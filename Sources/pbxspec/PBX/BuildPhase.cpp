/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/BuildPhase.h>

using pbxspec::PBX::BuildPhase;

BuildPhase::BuildPhase() :
    Specification()
{
}

BuildPhase::~BuildPhase()
{
}

BuildPhase::shared_ptr BuildPhase::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    BuildPhase::shared_ptr result;
    result.reset(new BuildPhase());

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool BuildPhase::
parse(Context *context, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildPhase",
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
        plist::MakeKey <plist::String> ("Version"));

    if (!Specification::parse(context, dict))
        return false;

    return true;
}

bool BuildPhase::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != BuildPhase::Type())
        return false;

    return inherit(reinterpret_cast <BuildPhase::shared_ptr const &> (base));
}

bool BuildPhase::
inherit(BuildPhase::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    return true;
}

