// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/ProductType.h>
#include <pbxspec/PBX/ApplicationProductType.h>
#include <pbxspec/PBX/BundleProductType.h>
#include <pbxspec/PBX/DynamicLibraryProductType.h>
#include <pbxspec/PBX/StaticLibraryProductType.h>

using pbxspec::PBX::ProductType;

ProductType::ProductType(bool isDefault) :
    ProductType(isDefault, ISA::PBXProductType)
{
}

ProductType::ProductType(bool isDefault, std::string const &isa) :
    Specification              (isa, isDefault),
    _defaultBuildProperties      (nullptr),
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

    if (_defaultBuildProperties != nullptr) {
        _defaultBuildProperties->release();
    }
}

ProductType::shared_ptr ProductType::
Parse(plist::Dictionary const *dict)
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
    } else if (C->value() == StaticLibraryProductType::Isa()) {
        result.reset(new StaticLibraryProductType(true));
    } else {
        fprintf(stderr, "warning: product type class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new ProductType(true));
    }
    if (!result->parse(dict))
        return nullptr;

    return result;
}

bool ProductType::
parse(plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "ProductType",
        // Specification
        plist::MakeKey <plist::String> ("Class"),
        plist::MakeKey <plist::String> ("Type"),
        plist::MakeKey <plist::String> ("Identifier"),
        plist::MakeKey <plist::String> ("BasedOn"),
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
        plist::MakeKey <plist::Boolean> ("AlwaysPerformSeparateStrip"));

    if (!Specification::parse(dict))
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

    if (DTN != nullptr) {
        _defaultTargetName = DTN->value();
    }

    if (DBP != nullptr) {
        if (_defaultBuildProperties != nullptr) {
            _defaultBuildProperties->release();
        }

        _defaultBuildProperties = plist::Copy(DBP);
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
    _defaultBuildProperties     = plist::Copy(base->defaultBuildProperties());
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
