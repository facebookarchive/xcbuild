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
    if (!ParseType(context, dict, Type())) {
        return nullptr;
    }

    BuildPhase::shared_ptr result;
    result.reset(new BuildPhase());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool BuildPhase::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("BuildPhase", dict, seen);

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

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

