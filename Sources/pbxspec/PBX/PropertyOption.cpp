/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/PropertyOption.h>

using pbxspec::PBX::PropertyOption;

PropertyOption::PropertyOption() :
    _basic                             (false),
    _commonOption                      (false),
    _avoidEmptyValues                  (false),
    _commandLineCondition              (true),
    _hasCommandLinePrefixFlag          (false),
    _commandLineArgs                   (nullptr),
    _additionalLinkerArgs              (nullptr),
    _defaultValue                      (nullptr),
    _allowedValues                     (nullptr),
    _values                            (nullptr),
    _isInputDependency                 (false),
    _isCommandInput                    (false),
    _isCommandOutput                   (false),
    _outputsAreSourceFiles             (false),
    _avoidMacroDefinition              (false),
    _flattenRecursiveSearchPathsInValue(false),
    _setValueInEnvironmentVariable     (pbxsetting::Value::Empty())
{
}

PropertyOption::~PropertyOption()
{
    if (_additionalLinkerArgs != nullptr) {
        _additionalLinkerArgs->release();
    }

    if (_values != nullptr) {
        _values->release();
    }

    if (_allowedValues != nullptr) {
        _allowedValues->release();
    }

    if (_defaultValue != nullptr) {
        _defaultValue->release();
    }

    if (_commandLineArgs != nullptr) {
        _commandLineArgs->release();
    }
}

pbxsetting::Setting PropertyOption::
defaultSetting(void) const
{
    return pbxsetting::Setting::Parse(_name, defaultPropertyValue().raw());
}

pbxsetting::Value PropertyOption::
defaultPropertyValue(void) const
{
    return pbxsetting::Value::FromObject(_defaultValue);
}

bool PropertyOption::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("Property/Option", dict, &seen);

    auto N      = unpack.cast <plist::String> ("Name");
    auto DN     = unpack.cast <plist::String> ("DisplayName");
    auto T      = unpack.cast <plist::String> ("Type");
    auto UT     = unpack.cast <plist::String> ("UIType");
    auto C      = unpack.cast <plist::String> ("Category");
    auto D      = unpack.cast <plist::String> ("Description");
    auto COND   = unpack.cast <plist::String> ("Condition");
    auto AA     = unpack.cast <plist::String> ("AppearsAfter");
    auto B      = unpack.coerce <plist::Boolean> ("Basic");
    auto CO     = unpack.coerce <plist::Boolean> ("CommonOption");
    auto AEV    = unpack.coerce <plist::Boolean> ("AvoidEmptyValue");
    auto CLC    = unpack.coerce <plist::Boolean> ("CommandLineCondition");
    auto CLA    = unpack.cast <plist::Object> ("CommandLineArgs");
    auto CLF    = unpack.cast <plist::String> ("CommandLineFlag");
    auto CLFIF  = unpack.cast <plist::String> ("CommandLineFlagIfFalse");
    auto CLPF   = unpack.cast <plist::String> ("CommandLinePrefixFlag");
    auto DV     = unpack.cast <plist::Object> ("DefaultValue");
    auto AV     = unpack.cast <plist::Object> ("AllowedValues");
    auto V      = unpack.cast <plist::Object> ("Values");
    auto FTs    = unpack.cast <plist::Array> ("FileTypes");
    auto CFs    = unpack.cast <plist::Array> ("ConditionFlavors");
    auto SVRs   = unpack.cast <plist::Array> ("SupportedVersionRanges");
    auto IID    = unpack.coerce <plist::Boolean> ("IsInputDependency");
    auto ICI    = unpack.coerce <plist::Boolean> ("IsCommandInput");
    auto ICO    = unpack.coerce <plist::Boolean> ("IsCommandOutput");
    auto AMD    = unpack.coerce <plist::Boolean> ("AvoidMacroDefinition");
    auto FRSPIV = unpack.coerce <plist::Boolean> ("FlattenRecursiveSearchPathsInValue");
    auto SVIEV  = unpack.cast <plist::String> ("SetValueInEnvironmentVariable");
    auto II     = unpack.cast <plist::String> ("InputInclusions");
    auto OD     = unpack.cast <plist::String> ("OutputDependencies");
    auto OASF   = unpack.coerce <plist::Boolean> ("OutputsAreSourceFiles");
    auto ALA    = unpack.cast <plist::Object> ("AdditionalLinkerArgs");
    auto As     = unpack.cast <plist::Array> ("Architectures");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (N != nullptr) {
        _name = N->value();
    }

    if (DN != nullptr) {
        _displayName = DN->value();
    }

    if (T != nullptr) {
        _type = T->value();
    }

    if (UT != nullptr) {
        _uiType = UT->value();
    }

    if (C != nullptr) {
        _category = C->value();
    }

    if (D != nullptr) {
        _description = D->value();
    }

    if (COND != nullptr) {
        _condition = COND->value();
    }

    if (AA != nullptr) {
        _appearsAfter = AA->value();
    }

    if (B != nullptr) {
        _basic = B->value();
    }

    if (CO != nullptr) {
        _commonOption = CO->value();
    }

    if (AEV != nullptr) {
        _avoidEmptyValues = AEV->value();
    }

    if (CLC != nullptr) {
        _commandLineCondition = CLC->value();
    }

    if (CLA != nullptr) {
        _commandLineArgs = CLA->copy().release();
    }

    if (CLF != nullptr) {
        _commandLineFlag = CLF->value();
    }

    if (CLFIF != nullptr) {
        _commandLineFlagIfFalse = CLFIF->value();
    }

    if (CLPF != nullptr) {
        _commandLinePrefixFlag = CLPF->value();
    }
    _hasCommandLinePrefixFlag = (CLPF != nullptr);

    if (DV != nullptr) {
        _defaultValue = DV->copy().release();
    }

    if (AV != nullptr) {
        _allowedValues = AV->copy().release();
    }

    if (V != nullptr) {
        _values = V->copy().release();
    }

    if (FTs != nullptr) {
        for (size_t n = 0; n < FTs->count(); n++) {
            if (auto FT = FTs->value <plist::String> (n)) {
                _fileTypes.push_back(FT->value());
            }
        }
    }

    if (CFs != nullptr) {
        for (size_t n = 0; n < CFs->count(); n++) {
            if (auto CF = CFs->value <plist::String> (n)) {
                _conditionFlavors.push_back(CF->value());
            }
        }
    }

    if (SVRs != nullptr) {
        for (size_t n = 0; n < SVRs->count(); n++) {
            if (auto SVR = SVRs->value <plist::String> (n)) {
                _supportedVersionRanges.push_back(SVR->value());
            }
        }
    }

    if (IID != nullptr) {
        _isInputDependency = IID->value();
    }

    if (ICI != nullptr) {
        _isCommandInput = ICI->value();
    }

    if (ICO != nullptr) {
        _isCommandOutput = ICO->value();
    }

    if (AMD != nullptr) {
        _avoidMacroDefinition = AMD->value();
    }

    if (FRSPIV != nullptr) {
        _flattenRecursiveSearchPathsInValue = FRSPIV->value();
    }

    if (SVIEV != nullptr) {
        _setValueInEnvironmentVariable = pbxsetting::Value::Parse(SVIEV->value());
    }

    if (II != nullptr) {
        _inputInclusions = II->value();
    }

    if (OD != nullptr) {
        _outputDependencies = OD->value();
    }

    if (OASF != nullptr) {
        _outputsAreSourceFiles = OASF->value();
    }

    if (ALA != nullptr) {
        _additionalLinkerArgs = ALA->copy().release();
    }

    if (As != nullptr) {
        for (size_t n = 0; n < As->count(); n++) {
            if (auto A = As->value <plist::String> (n)) {
                _architectures.push_back(A->value());
            }
        }
    }

    return true;
}
