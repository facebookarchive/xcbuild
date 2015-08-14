// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/CopyFilesBuildPhase.h>

using pbxproj::PBX::CopyFilesBuildPhase;

CopyFilesBuildPhase::CopyFilesBuildPhase() :
    BuildPhase       (Isa(), kTypeCopyFiles),
    _dstSubfolderSpec(0)
{
}

bool CopyFilesBuildPhase::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!BuildPhase::parse(context, dict))
        return false;

    auto DP  = dict->value <plist::String> ("dstPath");
    auto DSS = dict->value <plist::Integer> ("dstSubfolderSpec");

    if (DP != nullptr) {
        _dstPath = DP->value();
    }

    if (DSS != nullptr) {
        _dstSubfolderSpec = DSS->value();
    }

    return true;
}
