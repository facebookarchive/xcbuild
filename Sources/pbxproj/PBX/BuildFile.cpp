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
    std::string RPID;

    auto FR = context.indirect <FileReference> (dict, "fileRef", &FRID);
    auto RP = context.indirect <ReferenceProxy> (dict, "fileRef", &RPID);
    auto S  = dict->value <plist::Dictionary> ("settings");

    if (FR != nullptr) {
        _fileReference = context.parseObject(context.fileReferences, FRID, FR);
        if (!_fileReference) {
            return false;
        }
    } else if (RP != nullptr) {
        _referenceProxy = context.parseObject(context.referenceProxies, RPID, RP);
        if (!_referenceProxy) {
            return false;
        }
    }

    if (S != nullptr) {
        _settings = S->copy();
    }

    return true;
}
