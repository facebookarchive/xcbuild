/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
    auto IE = dict->value <plist::String> ("isEditable");
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
        _isEditable = (pbxsetting::Type::ParseInteger(IE->value()) != 0);
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
