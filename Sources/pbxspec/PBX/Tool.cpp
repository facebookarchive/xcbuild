/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/Tool.h>

using pbxspec::PBX::Tool;

Tool::Tool() :
    Specification                   (),
    _ruleName                       (pbxsetting::Value::Empty()),
    _ruleFormat                     (pbxsetting::Value::Empty()),
    _commandOutputParser            (nullptr),
    _isAbstract                     (false),
    _isArchitectureNeutral          (false),
    _caresAboutInclusionDependencies(false),
    _synthesizeBuildRule            (false),
    _shouldRerunOnError             (false),
    _deeplyStatInputDirectories     (false),
    _isUnsafeToInterrupt            (false),
    _messageLimit                   (0)
{
}

Tool::~Tool()
{
    if (_commandOutputParser != nullptr) {
        _commandOutputParser->release();
    }
}

pbxsetting::Level Tool::
defaultSettings(void) const
{
    std::vector<pbxsetting::Setting> settings;
    std::transform(_options.begin(), _options.end(), std::back_inserter(settings), [](PBX::PropertyOption::shared_ptr const &option) -> pbxsetting::Setting {
        return option->defaultSetting();
    });
    return pbxsetting::Level(settings);
}

Tool::shared_ptr Tool::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    Tool::shared_ptr result;
    result.reset(new Tool());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool Tool::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("Tool", dict, seen);

    auto EP     = unpack.cast <plist::String> ("ExecPath");
    auto ED     = unpack.cast <plist::String> ("ExecDescription");
    auto EDPC   = unpack.cast <plist::String> ("ExecDescriptionForPrecompile");
    auto EDC    = unpack.cast <plist::String> ("ExecDescriptionForCompile");
    auto EDCB   = unpack.cast <plist::String> ("ExecDescriptionForCreateBitcode");
    auto PD     = unpack.cast <plist::String> ("ProgressDescription");
    auto PDPC   = unpack.cast <plist::String> ("ProgressDescriptionForPrecompile");
    auto PDC    = unpack.cast <plist::String> ("ProgressDescriptionForCompile");
    auto PDCB   = unpack.cast <plist::String> ("ProgressDescriptionForCreateBitcode");
    auto CL     = unpack.cast <plist::String> ("CommandLine");
    auto CIC    = unpack.cast <plist::String> ("CommandInvocationClass");
    auto CI     = unpack.cast <plist::String> ("CommandIdentifier");
    auto RN     = unpack.cast <plist::String> ("RuleName");
    auto RF     = unpack.cast <plist::String> ("RuleFormat");
    auto AIF    = unpack.cast <plist::String> ("AdditionalInputFiles");
    auto BJRN   = unpack.cast <plist::String> ("BuiltinJambaseRuleName");
    auto FTs    = unpack.cast <plist::Array> ("FileTypes");
    auto IFTs   = unpack.cast <plist::Array> ("InputFileTypes");
    auto Os     = unpack.cast <plist::Array> ("Outputs");
    auto As     = unpack.cast <plist::Object> ("Architectures");
    auto EVs    = unpack.cast <plist::Dictionary> ("EnvironmentVariables");
    auto SECs   = unpack.cast <plist::Array> ("SuccessExitCodes");
    auto COP    = unpack.cast <plist::Object> ("CommandOutputParser");
    auto IA     = unpack.coerce <plist::Boolean> ("IsAbstract");
    auto IAN    = unpack.coerce <plist::Boolean> ("IsArchitectureNeutral");
    auto CAID   = unpack.coerce <plist::Boolean> ("CaresAboutInclusionDependencies");
    auto SBR    = unpack.coerce <plist::Boolean> ("SynthesizeBuildRule");
    auto SROE   = unpack.coerce <plist::Boolean> ("ShouldRerunOnError");
    auto DSID   = unpack.coerce <plist::Boolean> ("DeeplyStatInputDirectories");
    auto IUTI   = unpack.coerce <plist::Boolean> ("IsUnsafeToInterrupt");
    auto ML     = unpack.coerce <plist::Integer> ("MessageLimit");
    auto OPs    = unpack.cast <plist::Array> ("Options");
    auto DPs    = unpack.cast <plist::Array> ("DeletedProperties");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (EP != nullptr) {
        _execPath = EP->value();
    }

    if (ED != nullptr) {
        _execDescription = ED->value();
    }

    if (EDPC != nullptr) {
        _execDescriptionForPrecompile = EDPC->value();
    }

    if (EDC != nullptr) {
        _execDescriptionForCompile = EDC->value();
    }

    if (EDCB != nullptr) {
        _execDescriptionForCreateBitcode = EDCB->value();
    }

    if (PD != nullptr) {
        _progressDescription = PD->value();
    }

    if (PDPC != nullptr) {
        _progressDescriptionForPrecompile = PDPC->value();
    }

    if (PDC != nullptr) {
        _progressDescriptionForCompile = PDC->value();
    }

    if (PDCB != nullptr) {
        _progressDescriptionForCreateBitcode = PDCB->value();
    }

    if (CL != nullptr) {
        _commandLine = CL->value();
    }

    if (CIC != nullptr) {
        _commandInvocationClass = CIC->value();
    }

    if (CI != nullptr) {
        _commandIdentifier = CI->value();
    }

    if (RN != nullptr) {
        _ruleName = pbxsetting::Value::Parse(RN->value());
    }

    if (RF != nullptr) {
        _ruleFormat = pbxsetting::Value::Parse(RF->value());
    }

    if (AIF != nullptr) {
        _additionalInputFiles = AIF->value();
    }

    if (BJRN != nullptr) {
        _builtinJambaseRuleName = BJRN->value();
    }

    if (FTs != nullptr) {
        for (size_t n = 0; n < FTs->count(); n++) {
            if (auto FT = FTs->value <plist::String> (n)) {
                _fileTypes.push_back(FT->value());
            }
        }
    }

    if (IFTs != nullptr) {
        for (size_t n = 0; n < IFTs->count(); n++) {
            if (auto IFT = IFTs->value <plist::String> (n)) {
                _inputFileTypes.push_back(IFT->value());
            }
        }
    }

    if (auto AS = plist::CastTo<plist::Array>(As)) {
        for (size_t n = 0; n < AS->count(); n++) {
            if (auto A = AS->value <plist::String> (n)) {
                _architectures.push_back(A->value());
            }
        }
    } else if (auto AS = plist::CastTo<plist::String>(As)) {
        std::vector<std::string> values = pbxsetting::Type::ParseList(AS->value());
        _architectures.insert(_architectures.end(), values.begin(), values.end());
    }

    if (Os != nullptr) {
        for (size_t n = 0; n < Os->count(); n++) {
            if (auto O = Os->value <plist::String> (n)) {
                _outputs.push_back(O->value());
            }
        }
    }

    if (EVs != nullptr) {
        for (size_t n = 0; n < EVs->count(); n++) {
            auto EVk = EVs->key(n);
            if (auto EVv = EVs->value <plist::String> (EVk)) {
                _environmentVariables[EVk] = EVv->value();
            }
        }
    }

    if (SECs != nullptr) {
        for (size_t n = 0; n < SECs->count(); n++) {
            if (auto SEC = SECs->value <plist::String> (n)) {
                _successExitCodes.push_back(pbxsetting::Type::ParseInteger(SEC->value()));
            }
        }
    }

    if (COP != nullptr) {
        if (_commandOutputParser != nullptr) {
            _commandOutputParser->release();
        }

        _commandOutputParser = COP->copy().release();
    }

    if (IA != nullptr) {
        _isAbstract = IA->value();
    }

    if (IAN != nullptr) {
        _isArchitectureNeutral = IAN->value();
    }

    if (CAID != nullptr) {
        _caresAboutInclusionDependencies = CAID->value();
    }

    if (SBR != nullptr) {
        _synthesizeBuildRule = SBR->value();
    }

    if (SROE != nullptr) {
        _shouldRerunOnError = SROE->value();
    }

    if (DSID != nullptr) {
        _deeplyStatInputDirectories = DSID->value();
    }

    if (IUTI != nullptr) {
        _isUnsafeToInterrupt = IUTI->value();
    }

    if (ML != nullptr) {
        _messageLimit = ML->value();
    }

    if (OPs != nullptr) {
        for (size_t n = 0; n < OPs->count(); n++) {
            if (auto OP = OPs->value <plist::Dictionary> (n)) {
                PropertyOption::shared_ptr option;
                option.reset(new PropertyOption);
                if (option->parse(OP)) {
                    PropertyOption::Insert(&_options, &_optionsUsed, option);
                }
            }
        }
    }

    if (DPs != nullptr) {
        for (size_t n = 0; n < DPs->count(); n++) {
            if (auto DP = DPs->value <plist::String> (n)) {
                _deletedProperties.insert(DP->value());
            }
        }
    }

    return true;
}

bool Tool::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != Tool::Type())
        return false;

    return inherit(reinterpret_cast <Tool::shared_ptr const &> (base));
}

bool Tool::
inherit(Tool::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _execPath                            = base->execPath();
    _execDescription                     = base->execDescription();
    _execDescriptionForPrecompile        = base->execDescriptionForPrecompile();
    _execDescriptionForCompile           = base->execDescriptionForCompile();
    _execDescriptionForCreateBitcode     = base->execDescriptionForCreateBitcode();
    _progressDescription                 = base->progressDescription();
    _progressDescriptionForPrecompile    = base->progressDescriptionForPrecompile();
    _progressDescriptionForCompile       = base->progressDescriptionForCompile();
    _progressDescriptionForCreateBitcode = base->progressDescriptionForCreateBitcode();
    _commandLine                         = base->commandLine();
    _commandInvocationClass              = base->commandInvocationClass();
    _commandIdentifier                   = base->commandIdentifier();
    _ruleName                            = base->ruleName();
    _ruleFormat                          = base->ruleFormat();
    _additionalInputFiles                = base->additionalInputFiles();
    _builtinJambaseRuleName              = base->builtinJambaseRuleName();
    _fileTypes                           = base->fileTypes();
    _inputFileTypes                      = base->inputFileTypes();
    _architectures                       = base->architectures();
    _outputs                             = base->outputs();
    _environmentVariables                = base->environmentVariables();
    _commandOutputParser                 = base->commandOutputParser()->copy().release();
    _isAbstract                          = base->isAbstract();
    _isArchitectureNeutral               = base->isArchitectureNeutral();
    _caresAboutInclusionDependencies     = base->caresAboutInclusionDependencies();
    _synthesizeBuildRule                 = base->synthesizeBuildRule();
    _shouldRerunOnError                  = base->shouldRerunOnError();
    _deeplyStatInputDirectories          = base->deeplyStatInputDirectories();
    _isUnsafeToInterrupt                 = base->isUnsafeToInterrupt();
    _messageLimit                        = base->messageLimit();
    _options                             = base->options();
    _optionsUsed                         = base->_optionsUsed;
    _deletedProperties                   = base->deletedProperties();

    return true;
}
