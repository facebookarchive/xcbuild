/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/Architecture.h>

using pbxspec::PBX::Architecture;

Architecture::Architecture() :
    Specification(),
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
    return pbxsetting::Setting::Create(_architectureSetting, pbxsetting::Value::String(pbxsetting::Type::FormatList(_realArchitectures)));
}

Architecture::shared_ptr Architecture::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    Architecture::shared_ptr result;
    result.reset(new Architecture());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool Architecture::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("Architecture", dict, seen);

    auto RAs = unpack.cast <plist::Array> ("RealArchitectures");
    auto AS  = unpack.cast <plist::String> ("ArchitectureSetting");
    auto PAB = unpack.cast <plist::String> ("PerArchBuildSettingName");
    auto BO  = unpack.cast <plist::String> ("ByteOrder");
    auto LIE = unpack.coerce <plist::Boolean> ("ListInEnum");
    auto SN  = unpack.coerce <plist::Integer> ("SortNumber");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

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

    if (SN != nullptr) {
        _sortNumber = SN->value();
    }

    return true;
}

bool Architecture::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != Architecture::Type())
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
