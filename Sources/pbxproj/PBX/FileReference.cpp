// Copyright 2013-present Facebook. All Rights Reserved.

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
    auto III  = dict->value <plist::Integer> ("includeInIndex");
    auto FE   = dict->value <plist::Integer> ("fileEncoding");

    if (LKFT != nullptr) {
        _lastKnownFileType = LKFT->value();
    }

    if (EFT != nullptr) {
        _explicitFileType = EFT->value();
    }

    if (III != nullptr) {
        _includeInIndex = (III->value() != 0);
    }

    if (FE != nullptr) {
        _fileEncoding = FE->value();
    }

    return true;
}
