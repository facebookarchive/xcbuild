// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/BuildPhase.h>

using pbxproj::PBX::BuildPhase;

BuildPhase::BuildPhase(std::string const &isa, Type type) :
    Object                             (isa),
    _type                              (type),
    _runOnlyForDeploymentPostprocessing(false),
    _buildActionMask                   (0)
{
}

bool BuildPhase::
parse(Context &context, plist::Dictionary const *dict)
{
    auto Fs    = dict->value <plist::Array> ("files");
    auto ROFDP = dict->value <plist::Integer> ("runOnlyForDeploymentPostprocessing");
    auto BAM   = dict->value <plist::Integer> ("buildActionMask");

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
        _runOnlyForDeploymentPostprocessing = (ROFDP->value() != 0);
    }

    if (BAM != nullptr) {
        _buildActionMask = BAM->value();
    }

    return true;
}
