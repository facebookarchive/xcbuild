/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/Compiler.h>
#include <pbxspec/Manager.h>

using pbxspec::PBX::Compiler;

Compiler::Compiler() :
    Tool                                     (),
    _outputDir                               (pbxsetting::Value::Empty()),
    _generatedInfoPlistContentFilePath       (pbxsetting::Value::Empty()),
    _dependencyInfoFile                      (pbxsetting::Value::Empty()),
    _overridingProperties                    (pbxsetting::Level({ })),
    _useCPlusPlusCompilerDriverWhenBundlizing(false),
    _dashIFlagAcceptHeadermaps               (false),
    _supportsHeadermaps                      (false),
    _supportsIsysroot                        (false),
    _supportsSeparateUserHeaderPaths         (false),
    _supportsGeneratePreprocessedFile        (false),
    _supportsGenerateAssemblyFile            (false),
    _supportsAnalyzeFile                     (false),
    _supportsSerializedDiagnostics           (false),
    _supportsPredictiveCompilation           (false),
    _supportsMacOSXDeploymentTarget          (false),
    _supportsMacOSXMinVersionFlag            (false),
    _prunePrecompiledHeaderCache             (false),
    _outputAreProducts                       (false),
    _outputAreSourceFiles                    (false),
    _softError                               (false),
    _deeplyStatInputDirectories              (false),
    _dontProcessOutputs                      (false),
    _showInCompilerSelectionPopup            (false),
    _showOnlySelfDefinedProperties           (false),
    _mightNotEmitAllOutputs                  (false),
    _includeInUnionedToolDefaults            (false)
{
}

Compiler::~Compiler()
{
}

bool Compiler::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != Compiler::Type())
        return false;

    return inherit(reinterpret_cast <Compiler::shared_ptr const &> (base));
}

bool Compiler::
inherit(Tool::shared_ptr const &base)
{
    if (base->type() != Compiler::Type())
        return false;

    return inherit(reinterpret_cast <Compiler::shared_ptr const &> (base));
}

bool Compiler::
inherit(Compiler::shared_ptr const &b)
{
    if (!Tool::inherit(reinterpret_cast <Tool::shared_ptr const &> (b)))
        return false;

    auto base = this->base();

    _execCPlusPlusLinkerPath                  = base->execCPlusPlusLinkerPath();
    _executionDescription                     = base->executionDescription();
    _sourceFileOption                         = base->sourceFileOption();
    _outputDir                                = base->outputDir();
    _outputFileExtension                      = base->outputFileExtension();
    _commandResultsPostprocessor              = base->commandResultsPostprocessor();
    _genericCommandFailedErrorString          = base->genericCommandFailedErrorString();
    _generatedInfoPlistContentFilePath        = base->generatedInfoPlistContentFilePath();
    _dependencyInfoFile                       = base->dependencyInfoFile();
    _dependencyInfoArgs                       = base->dependencyInfoArgs();
    _languages                                = base->languages();
    _optionConditionFlavors                   = base->optionConditionFlavors();
    _patternsOfFlagsNotAffectingPrecomps      = base->patternsOfFlagsNotAffectingPrecomps();
    _messageCategoryInfoOptions               = base->messageCategoryInfoOptions();
    _synthesizeBuildRuleForBuildPhases        = base->synthesizeBuildRuleForBuildPhases();
    _inputFileGroupings                       = base->inputFileGroupings();
    _fallbackTools                            = base->fallbackTools();
    _additionalDirectoriesToCreate            = base->additionalDirectoriesToCreate();
    _overridingProperties                     = base->overridingProperties();
    _useCPlusPlusCompilerDriverWhenBundlizing = base->useCPlusPlusCompilerDriverWhenBundlizing();
    _supportsHeadermaps                       = base->supportsHeadermaps();
    _supportsIsysroot                         = base->supportsIsysroot();
    _supportsSeparateUserHeaderPaths          = base->supportsSeparateUserHeaderPaths();
    _supportsGeneratePreprocessedFile         = base->supportsGeneratePreprocessedFile();
    _supportsGenerateAssemblyFile             = base->supportsGenerateAssemblyFile();
    _supportsAnalyzeFile                      = base->supportsAnalyzeFile();
    _supportsSerializedDiagnostics            = base->supportsSerializedDiagnostics();
    _supportsPredictiveCompilation            = base->supportsPredictiveCompilation();
    _supportsMacOSXDeploymentTarget           = base->supportsMacOSXDeploymentTarget();
    _supportsMacOSXMinVersionFlag             = base->supportsMacOSXMinVersionFlag();
    _prunePrecompiledHeaderCache              = base->prunePrecompiledHeaderCache();
    _outputAreProducts                        = base->outputAreProducts();
    _outputAreSourceFiles                     = base->outputAreSourceFiles();
    _softError                                = base->softError();
    _deeplyStatInputDirectories               = base->deeplyStatInputDirectories();
    _dontProcessOutputs                       = base->dontProcessOutputs();
    _showInCompilerSelectionPopup             = base->showInCompilerSelectionPopup();
    _showOnlySelfDefinedProperties            = base->showOnlySelfDefinedProperties();
    _mightNotEmitAllOutputs                   = base->mightNotEmitAllOutputs();
    _includeInUnionedToolDefaults             = base->includeInUnionedToolDefaults();

    return true;
}

Compiler::shared_ptr Compiler::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    Compiler::shared_ptr result;
    result.reset(new Compiler());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool Compiler::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Tool::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("Compiler", dict, seen);

    auto ED       = unpack.cast <plist::String> ("ExecutionDescription");
    auto ECPPLP   = unpack.cast <plist::String> ("ExecCPlusPlusLinkerPath");
    auto SFO      = unpack.cast <plist::String> ("SourceFileOption");
    auto OD       = unpack.cast <plist::String> ("OutputDir");
    auto OFE      = unpack.cast <plist::String> ("OutputFileExtension");
    auto CRP      = unpack.cast <plist::String> ("CommandResultsPostprocessor");
    auto GCFES    = unpack.cast <plist::String> ("GenericCommandFailedErrorString");
    auto GIPCFP   = unpack.cast <plist::String> ("GeneratedInfoPlistContentFilePath");
    auto DIF      = unpack.cast <plist::String> ("DependencyInfoFile");
    auto DIAs     = unpack.cast <plist::Array> ("DependencyInfoArgs");
    auto Ls       = unpack.cast <plist::Array> ("Languages");
    auto OCFs     = unpack.cast <plist::Array> ("OptionConditionFlavors");
    auto POFNAPs  = unpack.cast <plist::Array> ("PatternsOfFlagsNotAffectingPrecomps");
    auto MCIOs    = unpack.cast <plist::Array> ("MessageCategoryInfoOptions");
    auto SBRFBPs  = unpack.cast <plist::Array> ("SynthesizeBuildRuleForBuildPhases");
    auto IFGs     = unpack.cast <plist::Array> ("InputFileGroupings");
    auto FTs      = unpack.cast <plist::Array> ("FallbackTools");
    auto ADTCs    = unpack.cast <plist::Array> ("AdditionalDirectoriesToCreate");
    auto OP       = unpack.cast <plist::Dictionary> ("OverridingProperties");
    auto UCPPCDWB = unpack.coerce <plist::Boolean> ("UseCPlusPlusCompilerDriverWhenBundlizing");
    auto DIFAH    = unpack.coerce <plist::Boolean> ("DashIFlagAcceptsHeadermaps");
    auto SH       = unpack.coerce <plist::Boolean> ("SupportsHeadermaps");
    auto SI       = unpack.coerce <plist::Boolean> ("SupportsIsysroot");
    auto SSUHP    = unpack.coerce <plist::Boolean> ("SupportsSeparateUserHeaderPaths");
    auto SGPF     = unpack.coerce <plist::Boolean> ("SupportsGeneratePreprocessedFile");
    auto SGAF     = unpack.coerce <plist::Boolean> ("SupportsGenerateAssemblyFile");
    auto SAF      = unpack.coerce <plist::Boolean> ("SupportsAnalyzeFile");
    auto SSD      = unpack.coerce <plist::Boolean> ("SupportsSerializedDiagnostics");
    auto SPC      = unpack.coerce <plist::Boolean> ("SupportsPredictiveCompilation");
    auto SMOSXDT  = unpack.coerce <plist::Boolean> ("SupportsMacOSXDeploymentTarget");
    auto SMOSXMVF = unpack.coerce <plist::Boolean> ("SupportsMacOSXMinVersionFlag");
    auto PPHC     = unpack.coerce <plist::Boolean> ("PrunePrecompiledHeaderCache");
    auto OAP      = unpack.coerce <plist::Boolean> ("OutputsAreProducts");
    auto OASF     = unpack.coerce <plist::Boolean> ("OutputsAreSourceFiles");
    auto SE       = unpack.coerce <plist::Boolean> ("SoftError");
    auto DSID     = unpack.coerce <plist::Boolean> ("DeeplyStatInputDirectories");
    auto DPO      = unpack.coerce <plist::Boolean> ("DontProcessOutputs");
    auto SICSP    = unpack.coerce <plist::Boolean> ("ShowInCompilerSelectionPopup");
    auto SOSDP    = unpack.coerce <plist::Boolean> ("ShowOnlySelfDefinedProperties");
    auto MNEAO    = unpack.coerce <plist::Boolean> ("MightNotEmitAllOutputs");
    auto IIUTD    = unpack.coerce <plist::Boolean> ("IncludeInUnionedToolDefaults");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (ECPPLP != nullptr) {
        _execCPlusPlusLinkerPath = ECPPLP->value();
    }

    if (ED != nullptr) {
        _executionDescription = ED->value();
    }

    if (SFO != nullptr) {
        _sourceFileOption = SFO->value();
    }

    if (OD != nullptr) {
        _outputDir = pbxsetting::Value::Parse(OD->value());
    }

    if (OFE != nullptr) {
        _outputFileExtension = OFE->value();
    }

    if (CRP != nullptr) {
        _commandResultsPostprocessor = CRP->value();
    }

    if (GCFES != nullptr) {
        _genericCommandFailedErrorString = GCFES->value();
    }

    if (GIPCFP != nullptr) {
        _generatedInfoPlistContentFilePath = pbxsetting::Value::Parse(GIPCFP->value());
    }

    if (DIF != nullptr) {
        _dependencyInfoFile = pbxsetting::Value::Parse(DIF->value());
    }

    if (DIAs != nullptr) {
        for (size_t n = 0; n < DIAs->count(); n++) {
            if (auto DIA = DIAs->value <plist::String> (n)) {
                _dependencyInfoArgs.push_back(pbxsetting::Value::Parse(DIA->value()));
            }
        }
    }

    if (Ls != nullptr) {
        for (size_t n = 0; n < Ls->count(); n++) {
            if (auto L = Ls->value <plist::String> (n)) {
                _languages.push_back(L->value());
            }
        }
    }

    if (OCFs != nullptr) {
        for (size_t n = 0; n < OCFs->count(); n++) {
            if (auto OCF = OCFs->value <plist::String> (n)) {
                _optionConditionFlavors.push_back(OCF->value());
            }
        }
    }

    if (POFNAPs != nullptr) {
        for (size_t n = 0; n < POFNAPs->count(); n++) {
            if (auto POFNAP = POFNAPs->value <plist::String> (n)) {
                _patternsOfFlagsNotAffectingPrecomps.push_back(POFNAP->value());
            }
        }
    }

    if (MCIOs != nullptr) {
        for (size_t n = 0; n < MCIOs->count(); n++) {
            if (auto MCIO = MCIOs->value <plist::String> (n)) {
                _messageCategoryInfoOptions.push_back(MCIO->value());
            }
        }
    }

    if (SBRFBPs != nullptr) {
        for (size_t n = 0; n < SBRFBPs->count(); n++) {
            if (auto SBRFBP = SBRFBPs->value <plist::String> (n)) {
                _synthesizeBuildRuleForBuildPhases.push_back(SBRFBP->value());
            }
        }
    }

    if (IFGs != nullptr) {
        for (size_t n = 0; n < IFGs->count(); n++) {
            if (auto IFG = IFGs->value <plist::String> (n)) {
                _inputFileGroupings.push_back(IFG->value());
            }
        }
    }

    if (FTs != nullptr) {
        for (size_t n = 0; n < FTs->count(); n++) {
            if (auto FT = FTs->value <plist::String> (n)) {
                _fallbackTools.push_back(FT->value());
            }
        }
    }

    if (ADTCs != nullptr) {
        for (size_t n = 0; n < ADTCs->count(); n++) {
            if (auto ADTC = ADTCs->value <plist::String> (n)) {
                _additionalDirectoriesToCreate.push_back(pbxsetting::Value::Parse(ADTC->value()));
            }
        }
    }

    if (OP != nullptr) {
        std::vector<pbxsetting::Setting> settings;
        for (size_t n = 0; n < OP->count(); n++) {
            auto OPK = OP->key(n);
            auto OPV = OP->value <plist::String> (OPK);

            if (OPV != nullptr) {
                pbxsetting::Setting setting = pbxsetting::Setting::Parse(OPK, OPV->value());
                settings.push_back(setting);
            }
        }
        _overridingProperties = pbxsetting::Level(settings);
    }

    if (UCPPCDWB != nullptr) {
        _useCPlusPlusCompilerDriverWhenBundlizing = UCPPCDWB->value();
    }

    if (SH != nullptr) {
        _supportsHeadermaps = SH->value();
    }

    if (SI != nullptr) {
        _supportsIsysroot = SI->value();
    }

    if (SSUHP != nullptr) {
        _supportsSeparateUserHeaderPaths = SSUHP->value();
    }

    if (SGPF != nullptr) {
        _supportsGeneratePreprocessedFile = SGPF->value();
    }

    if (SGAF != nullptr) {
        _supportsGenerateAssemblyFile = SGAF->value();
    }

    if (SAF != nullptr) {
        _supportsAnalyzeFile = SAF->value();
    }

    if (SSD != nullptr) {
        _supportsSerializedDiagnostics = SSD->value();
    }

    if (SPC != nullptr) {
        _supportsPredictiveCompilation = SPC->value();
    }

    if (SMOSXDT != nullptr) {
        _supportsMacOSXDeploymentTarget = SMOSXDT->value();
    }

    if (SMOSXMVF != nullptr) {
        _supportsMacOSXMinVersionFlag = SMOSXMVF->value();
    }

    if (PPHC != nullptr) {
        _prunePrecompiledHeaderCache = PPHC->value();
    }

    if (OAP != nullptr) {
        _outputAreProducts = OAP->value();
    }

    if (OASF != nullptr) {
        _outputAreSourceFiles = OASF->value();
    }

    if (SE != nullptr) {
        _softError = SE->value();
    }

    if (DSID != nullptr) {
        _deeplyStatInputDirectories = DSID->value();
    }

    if (DPO != nullptr) {
        _dontProcessOutputs = DPO->value();
    }

    if (SICSP != nullptr) {
        _showInCompilerSelectionPopup = SICSP->value();
    }

    if (SOSDP != nullptr) {
        _showOnlySelfDefinedProperties = SOSDP->value();
    }

    if (MNEAO != nullptr) {
        _mightNotEmitAllOutputs = MNEAO->value();
    }

    if (IIUTD != nullptr) {
        _includeInUnionedToolDefaults = IIUTD->value();
    }

    return true;
}
