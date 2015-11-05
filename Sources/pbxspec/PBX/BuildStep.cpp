/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/BuildStep.h>

using pbxspec::PBX::BuildStep;

BuildStep::BuildStep() :
    Specification()
{
}

BuildStep::~BuildStep()
{
}

BuildStep::shared_ptr BuildStep::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    BuildStep::shared_ptr result;
    result.reset(new BuildStep());

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool BuildStep::
parse(Context *context, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildStep",
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
        // BuildStep
        plist::MakeKey <plist::String> ("BuildStepType"));

    if (!Specification::parse(context, dict))
        return false;

    auto BST = dict->value <plist::String> ("BuildStepType");

    if (BST != nullptr) {
        _buildStepType = BST->value();
    }

    return true;
}

bool BuildStep::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != BuildStep::Type())
        return false;

    return inherit(reinterpret_cast <BuildStep::shared_ptr const &> (base));
}

bool BuildStep::
inherit(BuildStep::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _buildStepType = base->buildStepType();

    return true;
}

