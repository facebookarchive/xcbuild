/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/BuildRule.h>

using pbxspec::PBX::BuildRule;

BuildRule::
BuildRule()
{
}

BuildRule::
BuildRule(libutil::string_vector const &fileTypes, std::string const &compilerSpec) :
    _fileTypes(fileTypes),
    _compilerSpec(compilerSpec)
{
}

bool BuildRule::
parse(plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildRule",
        plist::MakeKey <plist::Object> ("Name"),
        plist::MakeKey <plist::String> ("FileType"),
        plist::MakeKey <plist::String> ("CompilerSpec")
        );

    auto N  = dict->value <plist::String> ("Name");
    auto FT = dict->value <plist::String> ("FileType");
    auto CS = dict->value <plist::String> ("CompilerSpec");

    if (N != nullptr) {
        _name = N->value();
    }

    if (FT != nullptr) {
        _fileTypes.push_back(FT->value());
    }

    if (CS != nullptr) {
        _compilerSpec = CS->value();
    }

    return true;
}
