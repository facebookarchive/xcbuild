// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/ProductType.h>
#include <pbxspec/PBX/ApplicationProductType.h>
#include <pbxspec/PBX/BundleProductType.h>
#include <pbxspec/PBX/DynamicLibraryProductType.h>
#include <pbxspec/PBX/FrameworkProductType.h>
#include <pbxspec/PBX/StaticLibraryProductType.h>
#include <pbxspec/XC/StaticFrameworkProductType.h>

using pbxspec::PBX::ProductType;
using pbxsetting::Level;
using pbxsetting::Setting;

ProductType::ProductType(bool isDefault) :
    ProductType(isDefault, ISA::PBXProductType)
{
}

ProductType::ProductType(bool isDefault, std::string const &isa) :
    Specification              (isa, isDefault),
    _defaultBuildProperties    (Level({ })),
    _validation                (nullptr),
    _hasInfoPlist              (false),
    _hasInfoPlistStrings       (false),
    _isWrapper                 (false),
    _supportsZeroLink          (false),
    _alwaysPerformSeparateStrip(false)
{
}

ProductType::~ProductType()
{
    if (_validation != nullptr) {
        _validation->release();
    }
}

ProductType::shared_ptr ProductType::
Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    ProductType::shared_ptr result;
    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new ProductType(true));
    } else if (C->value() == ApplicationProductType::Isa()) {
        result.reset(new ApplicationProductType(true));
    } else if (C->value() == BundleProductType::Isa()) {
        result.reset(new BundleProductType(true));
    } else if (C->value() == DynamicLibraryProductType::Isa()) {
        result.reset(new DynamicLibraryProductType(true));
    } else if (C->value() == FrameworkProductType::Isa()) {
        result.reset(new FrameworkProductType(true));
    } else if (C->value() == StaticLibraryProductType::Isa()) {
        result.reset(new StaticLibraryProductType(true));
    } else if (C->value() == XC::StaticFrameworkProductType::Isa()) {
        result.reset(new XC::StaticFrameworkProductType(true));
    } else {
        fprintf(stderr, "warning: product type class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new ProductType(true));
    }
    if (!result->parse(manager, dict))
        return nullptr;

    return result;
}

bool ProductType::
parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "ProductType",
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
        // ProductType
        plist::MakeKey <plist::String> ("DefaultTargetName"),
        plist::MakeKey <plist::Dictionary> ("DefaultBuildProperties"),
        plist::MakeKey <plist::Dictionary> ("Validation"),
        plist::MakeKey <plist::String> ("IconNamePrefix"),
        plist::MakeKey <plist::Array> ("PackageTypes"),
        plist::MakeKey <plist::Boolean> ("HasInfoPlist"),
        plist::MakeKey <plist::Boolean> ("HasInfoPlistStrings"),
        plist::MakeKey <plist::Boolean> ("IsWrapper"),
        plist::MakeKey <plist::Boolean> ("SupportsZeroLink"),
        plist::MakeKey <plist::Boolean> ("AlwaysPerformSeparateStrip"),
        plist::MakeKey <plist::Boolean> ("WantsSimpleTargetEditing"),
        plist::MakeKey <plist::Boolean> ("AddWatchCompanionRequirement"),
        plist::MakeKey <plist::Boolean> ("DisableSchemeAutocreation"),
        plist::MakeKey <plist::String> ("RunpathSearchPathForEmbeddedFrameworks"));

    if (!Specification::parse(manager, dict))
        return false;

    auto DTN  = dict->value <plist::String> ("DefaultTargetName");
    auto DBP  = dict->value <plist::Dictionary> ("DefaultBuildProperties");
    auto V    = dict->value <plist::Dictionary> ("Validation");
    auto INP  = dict->value <plist::String> ("IconNamePrefix");
    auto PTs  = dict->value <plist::Array> ("PackageTypes");
    auto HIP  = dict->value <plist::Boolean> ("HasInfoPlist");
    auto HIPS = dict->value <plist::Boolean> ("HasInfoPlistStrings");
    auto IW   = dict->value <plist::Boolean> ("IsWrapper");
    auto SZL  = dict->value <plist::Boolean> ("SupportsZeroLink");
    auto APSS = dict->value <plist::Boolean> ("AlwaysPerformSeparateStrip");
    auto WSTE = dict->value <plist::Boolean> ("WantsSimpleTargetEditing");
    auto AWCR = dict->value <plist::Boolean> ("AddWatchCompanionRequirement");
    auto DSA  = dict->value <plist::Boolean> ("DisableSchemeAutocreation");
    auto RSEF = dict->value <plist::String> ("RunpathSearchPathForEmbeddedFrameworks");

    if (DTN != nullptr) {
        _defaultTargetName = DTN->value();
    }

    if (DBP != nullptr) {
        std::vector<Setting> settings;
        for (size_t n = 0; n < DBP->count(); n++) {
            auto DBPK = DBP->key(n);
            auto DBPV = DBP->value <plist::String> (DBPK);

            if (DBPV != nullptr) {
                Setting setting = Setting::Parse(DBPK, DBPV->value());
                settings.push_back(setting);
            }
        }
        _defaultBuildProperties = Level(settings);
    }

    if (V != nullptr) {
        if (_validation != nullptr) {
            _validation->release();
        }

        _validation = plist::Copy(V);
    }

    if (INP != nullptr) {
        _iconNamePrefix = INP->value();
    }

    if (PTs != nullptr) {
        for (size_t n = 0; n < PTs->count(); n++) {
            if (auto PT = PTs->value <plist::String> (n)) {
                _packageTypes.push_back(PT->value());
            }
        }
    }

    if (HIP != nullptr) {
        _hasInfoPlist = HIP->value();
    }

    if (HIPS != nullptr) {
        _hasInfoPlistStrings = HIPS->value();
    }

    if (IW != nullptr) {
        _isWrapper = IW->value();
    }

    if (SZL != nullptr) {
        _supportsZeroLink = SZL->value();
    }

    if (APSS != nullptr) {
        _alwaysPerformSeparateStrip = APSS->value();
    }

    if (WSTE != nullptr) {
        _wantsSimpleTargetEditing = WSTE->value();
    }

    if (AWCR != nullptr) {
        _addWatchCompanionRequirement = AWCR->value();
    }

    if (DSA != nullptr) {
        _disableSchemeAutocreation = DSA->value();
    }

    if (RSEF != nullptr) {
        _runpathSearchPathForEmbeddedFrameworks = RSEF->value();
    }

    return true;
}

bool ProductType::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(ProductType::Isa()))
        return false;

    return inherit(reinterpret_cast <ProductType::shared_ptr const &> (base));
}

bool ProductType::
inherit(ProductType::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _defaultTargetName          = base->defaultTargetName();
    _defaultBuildProperties     = base->defaultBuildProperties();
    _validation                 = plist::Copy(base->validation());
    _iconNamePrefix             = base->iconNamePrefix();
    _packageTypes               = base->packageTypes();
    _hasInfoPlist               = base->hasInfoPlist();
    _hasInfoPlistStrings        = base->hasInfoPlistStrings();
    _isWrapper                  = base->isWrapper();
    _supportsZeroLink           = base->supportsZeroLink();
    _alwaysPerformSeparateStrip = base->alwaysPerformSeparateStrip();

    return true;
}
