// Copyright 2013-present Facebook. All Rights Reserved.

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
