/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/FileReference.h>

using pbxproj::PBX::FileReference;

FileReference::FileReference() :
    GroupItem      (Isa(), GroupItem::kTypeFileReference),
    _includeInIndex(false),
    _fileEncoding  (0)
{
}

bool FileReference::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!GroupItem::parse(context, dict))
        return false;

    auto LKFT = dict->value <plist::String> ("lastKnownFileType");
    auto EFT  = dict->value <plist::String> ("explicitFileType");
    auto III  = dict->value <plist::String> ("includeInIndex");
    auto FE   = dict->value <plist::String> ("fileEncoding");

    if (LKFT != nullptr) {
        _lastKnownFileType = LKFT->value();
    }

    if (EFT != nullptr) {
        _explicitFileType = EFT->value();
    }

    if (III != nullptr) {
        _includeInIndex = (pbxsetting::Type::ParseInteger(III->value()) != 0);
    }

    if (FE != nullptr) {
        _fileEncoding = pbxsetting::Type::ParseInteger(FE->value());
    }

    return true;
}
