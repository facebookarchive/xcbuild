// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/BuildPhase.h>

using pbxspec::PBX::BuildPhase;

BuildPhase::BuildPhase() :
    Specification(ISA::PBXBuildPhase)
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
    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new BuildPhase());
    } else {
        fprintf(stderr, "warning: build phase class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new BuildPhase());
    }
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
    if (!base->isa(BuildPhase::Isa()))
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

