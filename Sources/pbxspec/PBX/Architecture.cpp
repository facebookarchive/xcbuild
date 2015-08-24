// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/Architecture.h>

using pbxspec::PBX::Architecture;

Architecture::Architecture(bool isDefault) :
    Specification(ISA::PBXArchitecture, isDefault),
    _listInEnum  (false),
    _sortNumber  (0)
{
}

Architecture::~Architecture()
{
}

pbxsetting::Setting Architecture::
defaultSetting(void) const
{
    std::string value;
    for (std::string const &arch : _realArchitectures) {
        if (&arch != &_realArchitectures[0]) {
            value += " ";
        }
        value += arch;
    }

    return pbxsetting::Setting::Parse(_architectureSetting, value);
}

Architecture::shared_ptr Architecture::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    Architecture::shared_ptr result;
    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new Architecture(true));
    } else {
        fprintf(stderr, "warning: build phase class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new Architecture(true));
    }
    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool Architecture::
parse(Context *context, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "Architecture",
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
        // Architecture
        plist::MakeKey <plist::Array> ("RealArchitectures"),
        plist::MakeKey <plist::String> ("ArchitectureSetting"),
        plist::MakeKey <plist::String> ("PerArchBuildSettingName"),
        plist::MakeKey <plist::String> ("ByteOrder"),
        plist::MakeKey <plist::Boolean> ("ListInEnum"),
        plist::MakeKey <plist::Object> ("SortNumber"));

    if (!Specification::parse(context, dict))
        return false;

    auto RAs = dict->value <plist::Array> ("RealArchitectures");
    auto AS  = dict->value <plist::String> ("ArchitectureSetting");
    auto PAB = dict->value <plist::String> ("PerArchBuildSettingName");
    auto BO  = dict->value <plist::String> ("ByteOrder");
    auto LIE = dict->value <plist::Boolean> ("ListInEnum");
    auto SNi = dict->value <plist::Integer> ("SortNumber");
    auto SNs = dict->value <plist::String> ("SortNumber");

    if (RAs != nullptr) {
        for (size_t n = 0; n < RAs->count(); n++) {
            if (auto RA = RAs->value <plist::String> (n)) {
                _realArchitectures.push_back(RA->value());
            }
        }
    }

    if (AS != nullptr) {
        _architectureSetting = AS->value();
    }

    if (PAB != nullptr) {
        _perArchBuildSettingName = PAB->value();
    }

    if (BO != nullptr) {
        _byteOrder = BO->value();
    }

    if (LIE != nullptr) {
        _listInEnum = LIE->value();
    }

    if (SNi != nullptr) {
        _sortNumber = SNi->value();
    } else if (SNs != nullptr) {
        _sortNumber = std::stoi(SNs->value());
    }

    return true;
}

bool Architecture::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(Architecture::Isa()))
        return false;

    return inherit(reinterpret_cast <Architecture::shared_ptr const &> (base));
}

bool Architecture::
inherit(Architecture::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _realArchitectures       = base->realArchitectures();
    _architectureSetting     = base->architectureSetting();
    _perArchBuildSettingName = base->perArchBuildSettingName();
    _byteOrder               = base->byteOrder();
    _listInEnum              = base->listInEnum();
    _sortNumber              = base->sortNumber();

    return true;
}
