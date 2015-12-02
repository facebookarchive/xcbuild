/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/BuildPhase.h>

using pbxproj::PBX::BuildPhase;

BuildPhase::BuildPhase(std::string const &isa, Type type) :
    Object                             (isa),
    _type                              (type),
    _runOnlyForDeploymentPostprocessing(false),
    _buildActionMask                   (INT32_MAX)
{
}

bool BuildPhase::
parse(Context &context, plist::Dictionary const *dict)
{
    auto N     = dict->value <plist::String> ("name");
    auto Fs    = dict->value <plist::Array> ("files");
    auto ROFDP = dict->value <plist::String> ("runOnlyForDeploymentPostprocessing");
    auto BAM   = dict->value <plist::String> ("buildActionMask");

    if (N != nullptr) {
        _name = N->value();
    }

    if (Fs != nullptr) {
        for (size_t n = 0; n < Fs->count(); n++) {
            std::string FID;
            auto F = context.get <BuildFile> (Fs->value(n), &FID);
            if (F != nullptr) {
                auto BF = context.parseObject(context.buildFiles, FID, F);
                if (!BF)
                    return false;

                _files.push_back(BF);
            }
        }
    }

    if (ROFDP != nullptr) {
        _runOnlyForDeploymentPostprocessing = (pbxsetting::Type::ParseInteger(ROFDP->value()) != 0);
    }

    if (BAM != nullptr) {
        _buildActionMask = pbxsetting::Type::ParseInteger(BAM->value());
    }

    return true;
}
