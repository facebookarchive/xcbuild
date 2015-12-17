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
}

ProductType::shared_ptr ProductType::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    ProductType::shared_ptr result;
    result.reset(new ProductType());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool ProductType::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("BuildPhase", dict, seen);

    auto DTN  = unpack.cast <plist::String> ("DefaultTargetName");
    auto DBP  = unpack.cast <plist::Dictionary> ("DefaultBuildProperties");
    auto V    = unpack.cast <plist::Dictionary> ("Validation");
    auto INP  = unpack.cast <plist::String> ("IconNamePrefix");
    auto PTs  = unpack.cast <plist::Array> ("PackageTypes");
    auto HIP  = unpack.coerce <plist::Boolean> ("HasInfoPlist");
    auto HIPS = unpack.coerce <plist::Boolean> ("HasInfoPlistStrings");
    auto IW   = unpack.coerce <plist::Boolean> ("IsWrapper");
    auto SZL  = unpack.coerce <plist::Boolean> ("SupportsZeroLink");
    auto APSS = unpack.coerce <plist::Boolean> ("AlwaysPerformSeparateStrip");
    auto WSTE = unpack.coerce <plist::Boolean> ("WantsSimpleTargetEditing");
    auto AWCR = unpack.coerce <plist::Boolean> ("AddWatchCompanionRequirement");
    auto ROP  = unpack.coerce <plist::Boolean> ("RunsOnProxy");
    auto DSA  = unpack.coerce <plist::Boolean> ("DisableSchemeAutocreation");
    auto VEB  = unpack.coerce <plist::Boolean> ("ValidateEmbeddedBinaries");
    auto SODR = unpack.coerce <plist::Boolean> ("SupportsOnDemandResources");
    auto CEAL = unpack.coerce <plist::Boolean> ("CanEmbedAddressSanitizerLibraries");
    auto RSEF = unpack.coerce <plist::Boolean> ("RunpathSearchPathForEmbeddedFrameworks");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

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
        _validation.parse(V);
    }

    if (INP != nullptr) {
        _iconNamePrefix = INP->value();
    }

    if (PTs != nullptr) {
        _packageTypes.clear();

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
        _validateEmbeddedBinaries = VEB->value();
    }

    if (SODR != nullptr) {
        _supportsOnDemandResources = SODR->value();
    }

    if (CEAL != nullptr) {
        _canEmbedAddressSanitizerLibraries = CEAL->value();
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
    _validation                             = base->validation();
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

ProductType::Validation::
Validation()
{
}

bool ProductType::Validation::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("Validation", dict, &seen);

    auto VTS = unpack.cast <plist::String> ("ValidationToolSpec");
    auto Cs  = unpack.cast <plist::Dictionary> ("Checks");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (VTS != nullptr) {
        _validationToolSpec = VTS->value();;
    }

    if (Cs != nullptr) {
        for (size_t n = 0; n < Cs->count(); n++) {
            if (auto CV = Cs->value <plist::String> (n)) {
                auto C = ProductType::Validation::Check(Cs->key(n), CV->value());
                _checks.push_back(C);
            }
        }
    }

    return true;
}

ProductType::Validation::Check::
Check(std::string const &check, std::string const &description) :
    _check      (check),
    _description(description)
{
}

