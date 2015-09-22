// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/PackageType.h>

using pbxspec::PBX::PackageType;
using pbxsetting::Level;
using pbxsetting::Setting;

PackageType::PackageType() :
    Specification        (ISA::PBXPackageType),
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

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool PackageType::
parse(Context *context, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "PackageType",
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
        // PackageType
        plist::MakeKey <plist::Dictionary> ("ProductReference"),
        plist::MakeKey <plist::Dictionary> ("DefaultBuildSettings"));

    if (!Specification::parse(context, dict))
        return false;

    auto PR  = dict->value <plist::Dictionary> ("ProductReference");
    auto DBS = dict->value <plist::Dictionary> ("DefaultBuildSettings");

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
    if (!base->isa(PackageType::Isa()))
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
    plist::WarnUnhandledKeys(dict, "ProductReference",
        plist::MakeKey <plist::String> ("Name"),
        plist::MakeKey <plist::String> ("FileType"),
        plist::MakeKey <plist::Boolean> ("IsLaunchable"));

    auto N  = dict->value <plist::String> ("Name");
    auto FT = dict->value <plist::String> ("FileType");
    auto IL = dict->value <plist::Boolean> ("IsLaunchable");

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
