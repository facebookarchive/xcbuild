/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/CopyFilesBuildPhase.h>

using pbxproj::PBX::CopyFilesBuildPhase;

CopyFilesBuildPhase::CopyFilesBuildPhase() :
    BuildPhase       (Isa(), kTypeCopyFiles),
    _dstSubfolderSpec(kDestinationAbsolute)
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
        _dstSubfolderSpec = static_cast <Destination> (DSS->value());
    }

    return true;
}
