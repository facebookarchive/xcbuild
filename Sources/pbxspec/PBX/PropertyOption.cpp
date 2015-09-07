// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/PropertyOption.h>

using pbxspec::PBX::PropertyOption;

PropertyOption::PropertyOption() :
    _basic                             (false),
    _commonOption                      (false),
    _avoidEmptyValues                  (false),
    _commandLineCondition              (true),
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
    _flattenRecursiveSearchPathsInValue(false)
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
    if (_defaultValue == nullptr) {
        return pbxsetting::Value::Empty();
    } else if (plist::String const *stringValue = plist::CastTo <plist::String> (_defaultValue)) {
        return pbxsetting::Value::Parse(stringValue->value());
    } else if (plist::Boolean const *booleanValue = plist::CastTo <plist::Boolean> (_defaultValue)) {
        return pbxsetting::Value::Parse(booleanValue->value() ? "YES" : "NO");
    } else if (plist::Integer const *integerValue = plist::CastTo <plist::Integer> (_defaultValue)) {
        return pbxsetting::Value::Parse(std::to_string(integerValue->value()));
    } else {
        // TODO(grp): Handle additional types?
        fprintf(stderr, "Warning: Unknown value type for setting %s.\n", _name.c_str());
        return pbxsetting::Value::Empty();
    }
}

bool PropertyOption::
parse(plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "Property/Option",
            plist::MakeKey <plist::String> ("Name"),
            plist::MakeKey <plist::String> ("DisplayName"),
            plist::MakeKey <plist::String> ("Type"),
            plist::MakeKey <plist::String> ("UIType"),
            plist::MakeKey <plist::String> ("Category"),
            plist::MakeKey <plist::String> ("Description"),
            plist::MakeKey <plist::String> ("Condition"),
            plist::MakeKey <plist::String> ("AppearsAfter"),
            plist::MakeKey <plist::Boolean> ("Basic"),
            plist::MakeKey <plist::Boolean> ("CommonOption"),
            plist::MakeKey <plist::Boolean> ("AvoidEmptyValue"),
            plist::MakeKey <plist::Boolean> ("CommandLineCondition"),
            plist::MakeKey <plist::Object> ("CommandLineArgs"),
            plist::MakeKey <plist::String> ("CommandLineFlag"),
            plist::MakeKey <plist::String> ("CommandLineFlagIfFalse"),
            plist::MakeKey <plist::String> ("CommandLinePrefixFlag"),
            plist::MakeKey <plist::Object> ("DefaultValue"),
            plist::MakeKey <plist::Object> ("AllowedValues"),
            plist::MakeKey <plist::Object> ("Values"),
            plist::MakeKey <plist::Array> ("FileTypes"),
            plist::MakeKey <plist::Array> ("ConditionFlavors"),
            plist::MakeKey <plist::Boolean> ("IsInputDependency"),
            plist::MakeKey <plist::Boolean> ("IsCommandInput"),
            plist::MakeKey <plist::Boolean> ("IsCommandOutput"),
            plist::MakeKey <plist::Boolean> ("AvoidMacroDefinition"),
            plist::MakeKey <plist::Boolean> ("FlattenRecursiveSearchPathsInValue"),
            plist::MakeKey <plist::String> ("SetValueInEnvironmentVariable"),
            plist::MakeKey <plist::String> ("InputInclusions"),
            plist::MakeKey <plist::String> ("OutputDependencies"),
            plist::MakeKey <plist::Boolean> ("OutputsAreSourceFiles"),
            plist::MakeKey <plist::Object> ("AdditionalLinkerArgs"),
            plist::MakeKey <plist::Array> ("Architectures"));

    auto N      = dict->value <plist::String> ("Name");
    auto DN     = dict->value <plist::String> ("DisplayName");
    auto T      = dict->value <plist::String> ("Type");
    auto UT     = dict->value <plist::String> ("UIType");
    auto C      = dict->value <plist::String> ("Category");
    auto D      = dict->value <plist::String> ("Description");
    auto COND   = dict->value <plist::String> ("Condition");
    auto AA     = dict->value <plist::String> ("AppearsAfter");
    auto B      = dict->value <plist::Boolean> ("Basic");
    auto CO     = dict->value <plist::Boolean> ("CommonOption");
    auto AEV    = dict->value <plist::Boolean> ("AvoidEmptyValue");
    auto CLC    = dict->value <plist::Boolean> ("CommandLineCondition");
    auto CLA    = dict->value("CommandLineArgs");
    auto CLF    = dict->value <plist::String> ("CommandLineFlag");
    auto CLFIF  = dict->value <plist::String> ("CommandLineFlagIfFalse");
    auto CLPF   = dict->value <plist::String> ("CommandLinePrefixFlag");
    auto DV     = dict->value("DefaultValue");
    auto AV     = dict->value("AllowedValues");
    auto V      = dict->value("Values");
    auto FTs    = dict->value <plist::Array> ("FileTypes");
    auto CFs    = dict->value <plist::Array> ("ConditionFlavors");
    auto IID    = dict->value <plist::Boolean> ("IsInputDependency");
    auto ICI    = dict->value <plist::Boolean> ("IsCommandInput");
    auto ICO    = dict->value <plist::Boolean> ("IsCommandOutput");
    auto AMD    = dict->value <plist::Boolean> ("AvoidMacroDefinition");
    auto FRSPIV = dict->value <plist::Boolean> ("FlattenRecursiveSearchPathsInValue");
    auto SVIEV  = dict->value <plist::String> ("SetValueInEnvironmentVariable");
    auto II     = dict->value <plist::String> ("InputInclusions");
    auto OD     = dict->value <plist::String> ("OutputDependencies");
    auto OASF   = dict->value <plist::Boolean> ("OutputsAreSourceFiles");
    auto ALA    = dict->value("AdditionalLinkerArgs");
    auto As     = dict->value <plist::Array> ("Architectures");

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
        _commandLineArgs = CLA->copy();
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

    if (DV != nullptr) {
        _defaultValue = DV->copy();
    }

    if (AV != nullptr) {
        _allowedValues = AV->copy();
    }

    if (V != nullptr) {
        _values = V->copy();
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
        _setValueInEnvironmentVariable = SVIEV->value();
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
        _additionalLinkerArgs = ALA->copy();
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
