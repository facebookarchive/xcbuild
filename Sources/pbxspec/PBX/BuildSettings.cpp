/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/BuildSettings.h>

using pbxspec::PBX::BuildSettings;

BuildSettings::BuildSettings() :
    Specification()
{
}

BuildSettings::~BuildSettings()
{
}

BuildSettings::shared_ptr BuildSettings::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    BuildSettings::shared_ptr result;
    result.reset(new BuildSettings());

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool BuildSettings::
parse(Context *context, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildSettings",
        // Specification
        plist::MakeKey <plist::String> ("Class"),
        plist::MakeKey <plist::String> ("Type"),
        plist::MakeKey <plist::String> ("Identifier"),
        plist::MakeKey <plist::String> ("BasedOn"),
        plist::MakeKey <plist::String> ("Domain"),
        plist::MakeKey <plist::Boolean> ("IsGlobalDomainInUI"),
        plist::MakeKey <plist::String> ("Name"),
        plist::MakeKey <plist::String> ("Description"),
        plist::MakeKey <plist::String> ("Vendor"),
        plist::MakeKey <plist::String> ("Version"),
        // BuildSettings
        plist::MakeKey <plist::Array> ("Options"));

    if (!Specification::parse(context, dict))
        return false;

    auto Os = dict->value <plist::Array> ("Options");

    if (Os != nullptr) {
        for (size_t n = 0; n < Os->count(); n++) {
            if (auto O = Os->value <plist::Dictionary> (n)) {
                PropertyOption::shared_ptr option;
                option.reset(new PropertyOption);
                if (option->parse(O)) {
                    PropertyOption::Insert(&_options, &_optionsUsed, option);
                }
            }
        }
    }

    return true;
}

bool BuildSettings::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != BuildSettings::Type())
        return false;

    return inherit(reinterpret_cast <BuildSettings::shared_ptr const &> (base));
}

bool BuildSettings::
inherit(BuildSettings::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _options            = base->options();
    _optionsUsed        = base->_optionsUsed;

    return true;
}

