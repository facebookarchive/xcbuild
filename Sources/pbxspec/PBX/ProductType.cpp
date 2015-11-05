/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/ProductType.h>

using pbxspec::PBX::ProductType;
using pbxsetting::Level;
using pbxsetting::Setting;

ProductType::ProductType() :
    Specification                          (),
    _defaultBuildProperties                (Level({ })),
    _validation                            (nullptr),
    _hasInfoPlist                          (false),
    _hasInfoPlistStrings                   (false),
    _isWrapper                             (false),
    _supportsZeroLink                      (false),
    _alwaysPerformSeparateStrip            (false),
    _wantsSimpleTargetEditing              (false),
    _addWatchCompanionRequirement          (false),
    _runsOnProxy                           (false),
    _disableSchemeAutocreation             (false),
    _validateEmbeddedBinaries              (false),
    _supportsOnDemandResources             (false),
    _canEmbedAddressSanitizerLibraries     (false)
{
}

ProductType::~ProductType()
{
    if (_validation != nullptr) {
        _validation->release();
    }
}

ProductType::shared_ptr ProductType::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    ProductType::shared_ptr result;
    result.reset(new ProductType());

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool ProductType::
parse(Context *context, plist::Dictionary const *dict)
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
        plist::MakeKey <plist::Boolean> ("RunsOnProxy"),
        plist::MakeKey <plist::Boolean> ("DisableSchemeAutocreation"),
        plist::MakeKey <plist::Object> ("ValidateEmbeddedBinaries"),
        plist::MakeKey <plist::Object> ("SupportsOnDemandResources"),
        plist::MakeKey <plist::Object> ("CanEmbedAddressSanitizerLibraries"),
        plist::MakeKey <plist::String> ("RunpathSearchPathForEmbeddedFrameworks"));

    if (!Specification::parse(context, dict))
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
    auto ROP  = dict->value <plist::Boolean> ("RunsOnProxy");
    auto DSA  = dict->value <plist::Boolean> ("DisableSchemeAutocreation");
    auto VEB  = dict->value <plist::Object> ("ValidateEmbeddedBinaries");
    auto SODR = dict->value <plist::Object> ("SupportsOnDemandResources");
    auto CEAL = dict->value <plist::Object> ("CanEmbedAddressSanitizerLibraries");
    auto RSEF = dict->value <plist::String> ("RunpathSearchPathForEmbeddedFrameworks");

    if (DTN != nullptr) {
        _defaultTargetName = DTN->value();
    }

    if (DBP != nullptr) {
        std::vector<Setting> settings = _defaultBuildProperties.settings();
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

    if (ROP != nullptr) {
        _runsOnProxy = ROP->value();
    }

    if (DSA != nullptr) {
        _disableSchemeAutocreation = DSA->value();
    }

    if (VEB != nullptr) {
        if (auto VEBb = plist::CastTo <plist::Boolean> (VEB)) {
            _validateEmbeddedBinaries = VEBb->value();
        } else if (auto VEBs = plist::CastTo <plist::String> (VEB)) {
            _validateEmbeddedBinaries = (VEBs->value() == "YES");
        }
    }

    if (SODR != nullptr) {
        if (auto SODRb = plist::CastTo <plist::Boolean> (SODR)) {
            _supportsOnDemandResources = SODRb->value();
        } else if (auto SODRs = plist::CastTo <plist::String> (SODR)) {
            _supportsOnDemandResources = (SODRs->value() == "YES");
        }
    }

    if (CEAL != nullptr) {
        if (auto CEALb = plist::CastTo <plist::Boolean> (CEAL)) {
            _canEmbedAddressSanitizerLibraries = CEALb->value();
        } else if (auto CEALs = plist::CastTo <plist::String> (CEAL)) {
            _canEmbedAddressSanitizerLibraries = (CEALs->value() == "YES");
        }
    }

    if (RSEF != nullptr) {
        _runpathSearchPathForEmbeddedFrameworks = RSEF->value();
    }

    return true;
}

bool ProductType::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != ProductType::Type())
        return false;

    return inherit(reinterpret_cast <ProductType::shared_ptr const &> (base));
}

bool ProductType::
inherit(ProductType::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _defaultTargetName                      = base->defaultTargetName();
    _defaultBuildProperties                 = base->defaultBuildProperties();
    _validation                             = plist::Copy(base->validation());
    _iconNamePrefix                         = base->iconNamePrefix();
    _packageTypes                           = base->packageTypes();
    _hasInfoPlist                           = base->hasInfoPlist();
    _hasInfoPlistStrings                    = base->hasInfoPlistStrings();
    _isWrapper                              = base->isWrapper();
    _supportsZeroLink                       = base->supportsZeroLink();
    _alwaysPerformSeparateStrip             = base->alwaysPerformSeparateStrip();
    _wantsSimpleTargetEditing               = base->wantsSimpleTargetEditing();
    _addWatchCompanionRequirement           = base->addWatchCompanionRequirement();
    _runsOnProxy                            = base->runsOnProxy();
    _disableSchemeAutocreation              = base->disableSchemeAutocreation();
    _validateEmbeddedBinaries               = base->validateEmbeddedBinaries();
    _supportsOnDemandResources              = base->supportsOnDemandResources();
    _canEmbedAddressSanitizerLibraries      = base->canEmbedAddressSanitizerLibraries();
    _runpathSearchPathForEmbeddedFrameworks = base->runpathSearchPathForEmbeddedFrameworks();

    return true;
}
