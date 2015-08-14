// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/BuildRule.h>

using pbxproj::PBX::BuildRule;

BuildRule::BuildRule() :
    Object(Isa())
{
}

bool BuildRule::
parse(Context &context, plist::Dictionary const *dict)
{
    auto CS = dict->value <plist::String> ("compilerSpec");
    auto FP = dict->value <plist::String> ("filePatterns");
    auto FT = dict->value <plist::String> ("fileType");
    auto IE = dict->value <plist::Integer> ("isEditable");
    auto OF = dict->value <plist::Array> ("outputFiles");
    auto S  = dict->value <plist::String> ("script");

    if (CS != nullptr) {
        _compilerSpec = CS->value();
    }

    if (FP != nullptr) {
        _filePatterns = FP->value();
    }

    if (FT != nullptr) {
        _fileType = FT->value();
    }

    if (IE != nullptr) {
        _isEditable = (IE->value() != 0);
    }

    if (OF != nullptr) {
        for (size_t n = 0; n < OF->count(); n++) {
            auto FN = OF->value <plist::String> (n);
            if (FN != nullptr) {
                _outputFiles.push_back(FN->value());
            }
        }
    }

    if (S != nullptr) {
        _script = S->value();
    }

    return true;
}
