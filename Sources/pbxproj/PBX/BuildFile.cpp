// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/BuildFile.h>

using pbxproj::PBX::BuildFile;

BuildFile::BuildFile() :
    Object   (Isa()),
    _settings(nullptr)
{
}

BuildFile::~BuildFile()
{
    if (_settings != nullptr) {
        _settings->release();
    }
}

bool BuildFile::
parse(Context &context, plist::Dictionary const *dict)
{
    std::string FRID;

    auto FR = context.indirect <FileReference> (dict, "fileRef", &FRID);
    auto S  = dict->value <plist::Dictionary> ("settings");

    if (FR != nullptr) {
        _fileRef = context.parseObject(context.fileReferences, FRID, FR);
        if (!_fileRef)
            return false;
    }

    if (S != nullptr) {
        _settings = S->copy();
    }

    return true;
}
