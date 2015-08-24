// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/Tool.h>
#include <pbxspec/PBX/CompilerSpecificationLex.h>
#include <pbxspec/PBX/CompilerSpecificationOpenCL.h>
#include <pbxspec/PBX/CompilerSpecificationPbxCp.h>
#include <pbxspec/PBX/CompilerSpecificationRez.h>
#include <pbxspec/PBX/CompilerSpecificationYacc.h>
#include <pbxspec/XC/ToolSpecificationHeadermapGenerator.h>
#include <pbxspec/XC/ToolSpecificationCompilationDatabaseGenerator.h>
#include <pbxspec/XC/ProductPackagingUtilityToolSpecification.h>

using pbxspec::PBX::Tool;

Tool::Tool(bool isDefault) :
    Tool(isDefault, ISA::PBXTool)
{
}

Tool::Tool(bool isDefault, std::string const &isa) :
    Specification                   (isa, isDefault),
    _ruleName                       (nullptr),
    _commandOutputParser            (nullptr),
    _isAbstract                     (false),
    _isArchitectureNeutral          (false),
    _caresAboutInclusionDependencies(false),
    _synthesizeBuildRule            (false),
    _shouldRerunOnError             (false)
{
}

Tool::~Tool()
{
    if (_commandOutputParser != nullptr) {
        _commandOutputParser->release();
    }

    if (_ruleName != nullptr) {
        _ruleName->release();
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
Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    Tool::shared_ptr result;

    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new Tool(true));
    } else if (C->value() == CompilerSpecificationLex::Isa()) {
        result.reset(new CompilerSpecificationLex(true));
    } else if (C->value() == CompilerSpecificationOpenCL::Isa()) {
        result.reset(new CompilerSpecificationOpenCL(true));
    } else if (C->value() == CompilerSpecificationPbxCp::Isa()) {
        result.reset(new CompilerSpecificationPbxCp(true));
    } else if (C->value() == CompilerSpecificationRez::Isa()) {
        result.reset(new CompilerSpecificationRez(true));
    } else if (C->value() == CompilerSpecificationYacc::Isa()) {
        result.reset(new CompilerSpecificationYacc(true));
    } else if (C->value() == XC::ToolSpecificationHeadermapGenerator::Isa()) {
        result.reset(new XC::ToolSpecificationHeadermapGenerator(true));
    } else if (C->value() == XC::ToolSpecificationCompilationDatabaseGenerator::Isa()) {
        result.reset(new XC::ToolSpecificationCompilationDatabaseGenerator(true));
    } else if (C->value() == XC::ProductPackagingUtilityToolSpecification::Isa()) {
        result.reset(new XC::ProductPackagingUtilityToolSpecification(true));
    } else {
        fprintf(stderr, "warning: tool class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new Tool(true));
    }

    if (!result->parse(manager, dict))
        return nullptr;

    return result;
}

bool Tool::
parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict, bool check)
{
    if (check) {
        plist::WarnUnhandledKeys(dict, "Tool",
                // Specification
                plist::MakeKey <plist::String> ("Class"),
                plist::MakeKey <plist::String> ("Type"),
                plist::MakeKey <plist::String> ("Identifier"),
                plist::MakeKey <plist::String> ("BasedOn"),
                plist::MakeKey <plist::String> ("Name"),
                plist::MakeKey <plist::String> ("Description"),
                plist::MakeKey <plist::String> ("Vendor"),
                plist::MakeKey <plist::String> ("Version"),
                // Tool
                plist::MakeKey <plist::String> ("ExecPath"),
                plist::MakeKey <plist::String> ("ExecDescription"),
                plist::MakeKey <plist::String> ("ExecDescriptionForPrecompile"),
                plist::MakeKey <plist::String> ("ExecDescriptionForCompile"),
                plist::MakeKey <plist::String> ("ExecDescriptionForCreateBitcode"),
                plist::MakeKey <plist::String> ("ProgressDescription"),
                plist::MakeKey <plist::String> ("ProgressDescriptionForPrecompile"),
                plist::MakeKey <plist::String> ("ProgressDescriptionForCompile"),
                plist::MakeKey <plist::String> ("ProgressDescriptionForCreateBitcode"),
                plist::MakeKey <plist::String> ("CommandLine"),
                plist::MakeKey <plist::String> ("CommandInvocationClass"),
                plist::MakeKey <plist::String> ("CommandIdentifier"),
                plist::MakeKey <plist::Object> ("RuleName"),
                plist::MakeKey <plist::String> ("RuleFormat"),
                plist::MakeKey <plist::String> ("AdditionalInputFiles"),
                plist::MakeKey <plist::String> ("BuiltinJambaseRuleName"),
                plist::MakeKey <plist::Array> ("FileTypes"),
                plist::MakeKey <plist::Array> ("InputFileTypes"),
                plist::MakeKey <plist::Array> ("Outputs"),
                plist::MakeKey <plist::Array> ("Architectures"),
                plist::MakeKey <plist::Dictionary> ("EnvironmentVariables"),
                plist::MakeKey <plist::Object> ("CommandOutputParser"),
                plist::MakeKey <plist::Boolean> ("IsAbstract"),
                plist::MakeKey <plist::Boolean> ("IsArchitectureNeutral"),
                plist::MakeKey <plist::Boolean> ("CaresAboutInclusionDependencies"),
                plist::MakeKey <plist::Boolean> ("SynthesizeBuildRule"),
                plist::MakeKey <plist::Boolean> ("ShouldRerunOnError"),
                plist::MakeKey <plist::Array> ("Options"),
                plist::MakeKey <plist::Array> ("DeletedProperties"));
    }

    if (!Specification::parse(manager, dict))
        return false;

    auto EP     = dict->value <plist::String> ("ExecPath");
    auto ED     = dict->value <plist::String> ("ExecDescription");
    auto EDPC   = dict->value <plist::String> ("ExecDescriptionForPrecompile");
    auto EDC    = dict->value <plist::String> ("ExecDescriptionForCompile");
    auto EDCB   = dict->value <plist::String> ("ExecDescriptionForCreateBitcode");
    auto PD     = dict->value <plist::String> ("ProgressDescription");
    auto PDPC   = dict->value <plist::String> ("ProgressDescriptionForPrecompile");
    auto PDC    = dict->value <plist::String> ("ProgressDescriptionForCompile");
    auto PDCB   = dict->value <plist::String> ("ProgressDescriptionForCreateBitcode");
    auto CL     = dict->value <plist::String> ("CommandLine");
    auto CIC    = dict->value <plist::String> ("CommandInvocationClass");
    auto CI     = dict->value <plist::String> ("CommandIdentifier");
    auto RN     = dict->value("RuleName");
    auto RF     = dict->value <plist::String> ("RuleFormat");
    auto AIF    = dict->value <plist::String> ("AdditionalInputFiles");
    auto BJRN   = dict->value <plist::String> ("BuiltinJambaseRuleName");
    auto FTs    = dict->value <plist::Array> ("FileTypes");
    auto IFTs   = dict->value <plist::Array> ("InputFileTypes");
    auto Os     = dict->value <plist::Array> ("Outputs");
    auto As     = dict->value <plist::Array> ("Architectures");
    auto EVs    = dict->value <plist::Dictionary> ("EnvironmentVariables");
    auto COP    = dict->value("CommandOutputParser");
    auto IA     = dict->value <plist::Boolean> ("IsAbstract");
    auto IAN    = dict->value <plist::Boolean> ("IsArchitectureNeutral");
    auto CAID   = dict->value <plist::Boolean> ("CaresAboutInclusionDependencies");
    auto SBR    = dict->value <plist::Boolean> ("SynthesizeBuildRule");
    auto SROE   = dict->value <plist::Boolean> ("ShouldRerunOnError");
    auto OPs    = dict->value <plist::Array> ("Options");
    auto DPs    = dict->value <plist::Array> ("DeletedProperties");

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
        if (_ruleName != nullptr) {
            _ruleName->release();
        }

        _ruleName = RN->copy();
    }

    if (RF != nullptr) {
        _ruleFormat = RF->value();
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

    if (As != nullptr) {
        for (size_t n = 0; n < As->count(); n++) {
            if (auto A = As->value <plist::String> (n)) {
                _architectures.push_back(A->value());
            }
        }
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
                _environmentVariables.insert(std::make_pair(EVk, EVv->value()));
            }
        }
    }

    if (COP != nullptr) {
        if (_commandOutputParser != nullptr) {
            _commandOutputParser->release();
        }

        _commandOutputParser = COP->copy();
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

    if (OPs != nullptr) {
        for (size_t n = 0; n < OPs->count(); n++) {
            if (auto OP = OPs->value <plist::Dictionary> (n)) {
                PropertyOption::shared_ptr option;
                option.reset(new PropertyOption);
                if (option->parse(OP)) {
                    _options.push_back(option);
                }
            }
        }
    }

    if (DPs != nullptr) {
        for (size_t n = 0; n < DPs->count(); n++) {
            if (auto DP = DPs->value <plist::String> (n)) {
                _deletedProperties.push_back(DP->value());
            }
        }
    }

    return true;
}

bool Tool::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(Tool::Isa()))
        return false;

    return inherit(reinterpret_cast <Tool::shared_ptr const &> (base));
}

bool Tool::
inherit(Tool::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _execPath                        = base->execPath();
    _execDescription                 = base->execDescription();
    _commandLine                     = base->commandLine();
    _commandInvocationClass          = base->commandInvocationClass();
    _ruleName                        = plist::Copy(base->ruleName());
    _ruleFormat                      = base->ruleFormat();
    _additionalInputFiles            = base->additionalInputFiles();
    _builtinJambaseRuleName          = base->builtinJambaseRuleName();
    _fileTypes                       = base->fileTypes();
    _inputFileTypes                  = base->inputFileTypes();
    _architectures                   = base->architectures();
    _outputs                         = base->outputs();
    _environmentVariables            = base->environmentVariables();
    _commandOutputParser             = plist::Copy(base->commandOutputParser());
    _isAbstract                      = base->isAbstract();
    _isArchitectureNeutral           = base->isArchitectureNeutral();
    _caresAboutInclusionDependencies = base->caresAboutInclusionDependencies();
    _synthesizeBuildRule             = base->synthesizeBuildRule();
    _shouldRerunOnError              = base->shouldRerunOnError();
    _options                         = base->options();
    _deletedProperties               = base->deletedProperties();

    return true;
}
