/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/PackageType.h>

using pbxspec::PBX::PackageType;
using pbxsetting::Level;
using pbxsetting::Setting;

PackageType::PackageType() :
    Specification        (),
    _defaultBuildSettings(Level({ }))
{
}

PackageType::~PackageType()
{
}

PackageType::shared_ptr PackageType::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    PackageType::shared_ptr result;
    result.reset(new PackageType());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool PackageType::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("PackageType", dict, seen);

    auto PR  = unpack.cast <plist::Dictionary> ("ProductReference");
    auto DBS = unpack.cast <plist::Dictionary> ("DefaultBuildSettings");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (PR != nullptr) {
        _productReference.reset(new ProductReference);
        if (!_productReference->parse(PR)) {
            _productReference.reset();
        }
    }

    if (DBS != nullptr) {
        std::vector<Setting> settings = _defaultBuildSettings.settings();
        for (size_t n = 0; n < DBS->count(); n++) {
            auto DBSK = DBS->key(n);
            auto DBSV = DBS->value <plist::String> (DBSK);

            if (DBSV != nullptr) {
                Setting setting = Setting::Parse(DBSK, DBSV->value());
                settings.push_back(setting);
            }
        }
        _defaultBuildSettings = Level(settings);
    }

    return true;
}

bool PackageType::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != PackageType::Type())
        return false;

    return inherit(reinterpret_cast <PackageType::shared_ptr const &> (base));
}

bool PackageType::
inherit(PackageType::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _productReference     = base->productReference();
    _defaultBuildSettings = base->defaultBuildSettings();

    return true;
}

PackageType::ProductReference::ProductReference() :
    _isLaunchable(false)
{
}

bool PackageType::ProductReference::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("ProductReference", dict, &seen);

    auto N  = unpack.cast <plist::String> ("Name");
    auto FT = unpack.cast <plist::String> ("FileType");
    auto IL = unpack.coerce <plist::Boolean> ("IsLaunchable");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (N != nullptr) {
        _name = N->value();
    }

    if (FT != nullptr) {
        _fileType = FT->value();
    }

    if (IL != nullptr) {
        _isLaunchable = IL->value();
    }

    return true;
}
