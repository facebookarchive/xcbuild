/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/NativeTarget.h>
#include <pbxproj/PBX/BuildPhases.h>

using pbxproj::PBX::NativeTarget;

NativeTarget::NativeTarget() :
    Target(Isa(), kTypeNative)
{
}

bool NativeTarget::
parse(Context &context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Target::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("NativeTarget", dict, seen);

    std::string PRID;

    auto PR  = context.indirect <FileReference> (&unpack, "productReference", &PRID);
    auto PT  = unpack.cast <plist::String> ("productType");
    auto BRs = unpack.cast <plist::Array> ("buildRules");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (PT != nullptr) {
        _productType = PT->value();
    }

    if (PR != nullptr) {
        _productReference =
          context.parseObject(context.fileReferences, PRID, PR);
        if (!_productReference) {
            abort();
            return false;
        }
    }

    if (BRs != nullptr) {
        for (size_t n = 0; n < BRs->count(); n++) {
            std::string BRID;
            auto BRd = context.get <BuildRule> (BRs->value(n), &BRID);
            if (BRd != nullptr) {
                auto BR = context.parseObject(context.buildRules, BRID, BRd);
                if (!BR) {
                    abort();
                    return false;
                }

                _buildRules.push_back(BR);
            }
        }
    }

    return true;
}
