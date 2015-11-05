/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
