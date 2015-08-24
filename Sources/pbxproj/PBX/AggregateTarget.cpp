// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/AggregateTarget.h>
#include <pbxproj/PBX/BuildPhases.h>

using pbxproj::PBX::AggregateTarget;

AggregateTarget::AggregateTarget() :
    Target(Isa(), kTypeAggregate)
{
}

bool AggregateTarget::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!Target::parse(context, dict))
        return false;

    auto PN  = dict->value <plist::String> ("productName");

    if (PN != nullptr) {
        _productName = PN->value();
    }

    return true;
}
