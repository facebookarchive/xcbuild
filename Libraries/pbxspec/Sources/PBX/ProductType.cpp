/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/ProductType.h>
#include <pbxspec/Inherit.h>
#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <plist/Keys/Unpack.h>

using pbxspec::PBX::ProductType;
using pbxsetting::Level;
using pbxsetting::Setting;

ProductType::
ProductType() :
    Specification()
{
}

ProductType::
~ProductType()
{
}

ProductType::shared_ptr ProductType::
Parse(Context *context, plist::Dictionary const *dict)
{
    if (!ParseType(context, dict, Type())) {
        return nullptr;
    }

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

    auto unpack = plist::Keys::Unpack("ProductType", dict, seen);

    auto DTN   = unpack.cast <plist::String> ("DefaultTargetName");
    auto DBP   = unpack.cast <plist::Dictionary> ("DefaultBuildProperties");
    auto V     = unpack.cast <plist::Dictionary> ("Validation");
    auto INP   = unpack.cast <plist::String> ("IconNamePrefix");
    auto PTs   = unpack.cast <plist::Array> ("PackageTypes");
    auto HIP   = unpack.coerce <plist::Boolean> ("HasInfoPlist");
    auto HIPS  = unpack.coerce <plist::Boolean> ("HasInfoPlistStrings");
    auto IW    = unpack.coerce <plist::Boolean> ("IsWrapper");
    auto IJ    = unpack.coerce <plist::Boolean> ("IsJava");
    auto SZL   = unpack.coerce <plist::Boolean> ("SupportsZeroLink");
    auto APSS  = unpack.coerce <plist::Boolean> ("AlwaysPerformSeparateStrip");
    auto WSTE  = unpack.coerce <plist::Boolean> ("WantsSimpleTargetEditing");
    auto AWCR  = unpack.coerce <plist::Boolean> ("AddWatchCompanionRequirement");
    auto ROP   = unpack.coerce <plist::Boolean> ("RunsOnProxy");
    auto DSA   = unpack.coerce <plist::Boolean> ("DisableSchemeAutocreation");
    auto VEB   = unpack.coerce <plist::Boolean> ("ValidateEmbeddedBinaries");
    auto SODR  = unpack.coerce <plist::Boolean> ("SupportsOnDemandResources");
    auto CEAL  = unpack.coerce <plist::Boolean> ("CanEmbedAddressSanitizerLibraries");
    auto RSEF  = unpack.coerce <plist::String> ("RunpathSearchPathForEmbeddedFrameworks");
    auto IE    = unpack.coerce <plist::Boolean> ("IsEmbeddable");
    auto BPIWE = unpack.coerce <plist::Array> ("BuildPhaseInjectionsWhenEmbedding");
    auto RBPD  = unpack.coerce <plist::String> ("RequiredBuiltProductsDir");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

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
        _validation = Validation();
        _validation->parse(V);
    }

    if (INP != nullptr) {
        _iconNamePrefix = INP->value();
    }

    if (PTs != nullptr) {
        _packageTypes = std::vector<std::string>();
        for (size_t n = 0; n < PTs->count(); n++) {
            if (auto PT = PTs->value <plist::String> (n)) {
                _packageTypes->push_back(PT->value());
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

    if (IJ != nullptr) {
        _isJava = IJ->value();
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

    if (IE != nullptr) {
        _isEmbeddable = IE->value();
    }

    if (BPIWE != nullptr) {
        _buildPhaseInjectionsWhenEmbedding = std::vector<BuildPhaseInjection>();
        for (size_t n = 0; n < BPIWE->count(); n++) {
            auto BPI = BPIWE->value <plist::Dictionary> (n);
            if (BPI != nullptr) {
                BuildPhaseInjection injection;
                if (injection.parse(BPI)) {
                    _buildPhaseInjectionsWhenEmbedding->push_back(injection);
                }
            }
        }
    }

    if (RBPD != nullptr) {
        _requiredBuiltProductsDir = RBPD->value();
    }

    return true;
}

bool ProductType::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != ProductType::Type())
        return false;

    return inherit(std::static_pointer_cast<ProductType>(base));
}

bool ProductType::
inherit(ProductType::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _defaultTargetName                      = Inherit::Override(_defaultTargetName, base->_defaultTargetName);
    _defaultBuildProperties                 = Inherit::Combine(_defaultBuildProperties, base->_defaultBuildProperties);
    _validation                             = Inherit::Override(_validation, base->_validation);
    _iconNamePrefix                         = Inherit::Override(_iconNamePrefix, base->_iconNamePrefix);
    _packageTypes                           = Inherit::Override(_packageTypes, base->_packageTypes);
    _hasInfoPlist                           = Inherit::Override(_hasInfoPlist, base->_hasInfoPlist);
    _hasInfoPlistStrings                    = Inherit::Override(_hasInfoPlistStrings, base->_hasInfoPlistStrings);
    _isWrapper                              = Inherit::Override(_isWrapper, base->_isWrapper);
    _isJava                                 = Inherit::Override(_isJava, base->_isJava);
    _supportsZeroLink                       = Inherit::Override(_supportsZeroLink, base->_supportsZeroLink);
    _alwaysPerformSeparateStrip             = Inherit::Override(_alwaysPerformSeparateStrip, base->_alwaysPerformSeparateStrip);
    _wantsSimpleTargetEditing               = Inherit::Override(_wantsSimpleTargetEditing, base->_wantsSimpleTargetEditing);
    _addWatchCompanionRequirement           = Inherit::Override(_addWatchCompanionRequirement, base->_addWatchCompanionRequirement);
    _runsOnProxy                            = Inherit::Override(_runsOnProxy, base->_runsOnProxy);
    _disableSchemeAutocreation              = Inherit::Override(_disableSchemeAutocreation, base->_disableSchemeAutocreation);
    _validateEmbeddedBinaries               = Inherit::Override(_validateEmbeddedBinaries, base->_validateEmbeddedBinaries);
    _supportsOnDemandResources              = Inherit::Override(_supportsOnDemandResources, base->_supportsOnDemandResources);
    _canEmbedAddressSanitizerLibraries      = Inherit::Override(_canEmbedAddressSanitizerLibraries, base->_canEmbedAddressSanitizerLibraries);
    _runpathSearchPathForEmbeddedFrameworks = Inherit::Override(_runpathSearchPathForEmbeddedFrameworks, base->_runpathSearchPathForEmbeddedFrameworks);
    _isEmbeddable                           = Inherit::Override(_isEmbeddable, base->_isEmbeddable);
    _buildPhaseInjectionsWhenEmbedding      = Inherit::Combine(_buildPhaseInjectionsWhenEmbedding, base->_buildPhaseInjectionsWhenEmbedding);
    _requiredBuiltProductsDir               = Inherit::Override(_requiredBuiltProductsDir, base->_requiredBuiltProductsDir);

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
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (VTS != nullptr) {
        _validationToolSpec = VTS->value();
    }

    if (Cs != nullptr) {
        _checks = std::vector<ProductType::Validation::Check>();
        for (size_t n = 0; n < Cs->count(); n++) {
            if (auto CV = Cs->value <plist::String> (n)) {
                auto C = ProductType::Validation::Check(Cs->key(n), CV->value());
                _checks->push_back(C);
            }
        }
    }

    return true;
}

ProductType::Validation::Check::
Check(ext::optional<std::string> const &check, ext::optional<std::string> const &description) :
    _check      (check),
    _description(description)
{
}

