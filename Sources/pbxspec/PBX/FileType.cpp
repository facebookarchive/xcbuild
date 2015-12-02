/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/FileType.h>
#include <pbxspec/Manager.h>

using pbxspec::PBX::FileType;

FileType::FileType() :
    _isTextFile                             (false),
    _isBuildPropertiesFile                  (false),
    _isSourceCode                           (false),
    _isPreprocessed                         (false),
    _isTransparent                          (false),
    _isDocumentation                        (false),
    _isEmbeddable                           (false),
    _isExecutable                           (false),
    _isExecutableWithGUI                    (false),
    _isApplication                          (false),
    _isBundle                               (false),
    _isLibrary                              (false),
    _isDynamicLibrary                       (false),
    _isStaticLibrary                        (false),
    _isFolder                               (false),
    _isWrappedFolder                        (false),
    _isFrameworkWrapper                     (false),
    _isStaticFrameworkWrapper               (false),
    _isProjectWrapper                       (false),
    _isTargetWrapper                        (false),
    _isScannedForIncludes                   (false),
    _includeInIndex                         (false),
    _canSetIncludeInIndex                   (false),
    _requiresHardTabs                       (false),
    _containsNativeCode                     (false),
    _appliesToBuildRules                    (false),
    _changesCauseDependencyGraphInvalidation(false),
    _codeSignOnCopy                         (false),
    _removeHeadersOnCopy                    (false),
    _validateOnCopy                         (false)
{
}

FileType::~FileType()
{
    //
    // Clear circular dependencies.
    //
    for (auto cp : _componentParts) {
        cp.second.reset();
    }
}

bool FileType::
inherit(Specification::shared_ptr const &base)
{
    if (base->type() != FileType::Type())
        return false;

    return inherit(reinterpret_cast <FileType::shared_ptr const &> (base));
}

bool FileType::
inherit(FileType::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _uti                                     = base->UTI();
    _extensions                              = base->extensions();
    _mimeTypes                               = base->MIMETypes();
    _typeCodes                               = base->typeCodes();
    _filenamePatterns                        = base->filenamePatterns();
    _magicWords                              = base->magicWords();
    _language                                = base->language();
    _computerLanguage                        = base->computerLanguage();
    _gccDialectName                          = base->GCCDialectName();
    _prefix                                  = base->prefix();
    _permissions                             = base->permissions();
    _buildPhaseInjectionsWhenEmbedding       = base->buildPhaseInjectionsWhenEmbedding();
    _appliesToBuildRules                     = base->appliesToBuildRules();
    _isTextFile                              = base->isTextFile();
    _isBuildPropertiesFile                   = base->isBuildPropertiesFile();
    _isSourceCode                            = base->isSourceCode();
    _isDocumentation                         = base->isDocumentation();
    _isPreprocessed                          = base->isPreprocessed();
    _isTransparent                           = base->isTransparent();
    _isEmbeddable                            = base->isEmbeddable();
    _isExecutable                            = base->isExecutable();
    _isExecutableWithGUI                     = base->isExecutableWithGUI();
    _isApplication                           = base->isApplication();
    _isBundle                                = base->isBundle();
    _isLibrary                               = base->isLibrary();
    _isDynamicLibrary                        = base->isDynamicLibrary();
    _isStaticLibrary                         = base->isStaticLibrary();
    _isFolder                                = base->isFolder();
    _isWrappedFolder                         = base->isWrappedFolder();
    _isScannedForIncludes                    = base->isScannedForIncludes();
    _isFrameworkWrapper                      = base->isFrameworkWrapper();
    _isStaticFrameworkWrapper                = base->isStaticFrameworkWrapper();
    _isProjectWrapper                        = base->isProjectWrapper();
    _isTargetWrapper                         = base->isTargetWrapper();
    _componentParts                          = base->componentParts();
    _extraPropertyNames                      = base->extraPropertyNames();
    _includeInIndex                          = base->includeInIndex();
    _canSetIncludeInIndex                    = base->canSetIncludeInIndex();
    _requiresHardTabs                        = base->requiresHardTabs();
    _containsNativeCode                      = base->containsNativeCode();
    _plistStructureDefinition                = base->plistStructureDefinition();
    _changesCauseDependencyGraphInvalidation = base->changesCauseDependencyGraphInvalidation();
    _fallbackAutoroutingBuildPhase           = base->fallbackAutoroutingBuildPhase();
    _codeSignOnCopy                          = base->codeSignOnCopy();
    _removeHeadersOnCopy                     = base->removeHeadersOnCopy();
    _validateOnCopy                          = base->validateOnCopy();

    return true;
}

FileType::shared_ptr FileType::
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    FileType::shared_ptr result;
    result.reset(new FileType());

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool FileType::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("FileType", dict, seen);

    auto MTs   = unpack.cast <plist::Array> ("MIMETypes");
    auto U     = unpack.cast <plist::String> ("UTI");
    auto Es    = unpack.cast <plist::Array> ("Extensions");
    auto TCs   = unpack.cast <plist::Array> ("TypeCodes");
    auto FPs   = unpack.cast <plist::Array> ("FilenamePatterns");
    auto MWs   = unpack.cast <plist::Array> ("MagicWord");
    auto L     = unpack.cast <plist::String> ("Language");
    auto CL    = unpack.cast <plist::String> ("ComputerLanguage");
    auto GDN   = unpack.cast <plist::String> ("GccDialectName");
    auto Ps    = unpack.cast <plist::Array> ("Prefix");
    auto P     = unpack.cast <plist::String> ("Permissions");
    auto BPIWE = unpack.cast <plist::Array> ("BuildPhaseInjectionsWhenEmbedding");
    auto ATBR  = unpack.coerce <plist::Boolean> ("AppliesToBuildRules");
    auto ITF   = unpack.coerce <plist::Boolean> ("IsTextFile");
    auto IBPF  = unpack.coerce <plist::Boolean> ("IsBuildPropertiesFile");
    auto ISC   = unpack.coerce <plist::Boolean> ("IsSourceCode");
    auto IP    = unpack.coerce <plist::Boolean> ("IsPreprocessed");
    auto IT    = unpack.coerce <plist::Boolean> ("IsTransparent");
    auto ID    = unpack.coerce <plist::Boolean> ("IsDocumentation");
    auto IEM   = unpack.coerce <plist::Boolean> ("IsEmbeddable");
    auto IE    = unpack.coerce <plist::Boolean> ("IsExecutable");
    auto IEWG  = unpack.coerce <plist::Boolean> ("IsExecutableWithGUI");
    auto IA    = unpack.coerce <plist::Boolean> ("IsApplication");
    auto IB    = unpack.coerce <plist::Boolean> ("IsBundle");
    auto IL    = unpack.coerce <plist::Boolean> ("IsLibrary");
    auto IDL   = unpack.coerce <plist::Boolean> ("IsDynamicLibrary");
    auto ISL   = unpack.coerce <plist::Boolean> ("IsStaticLibrary");
    auto IF    = unpack.coerce <plist::Boolean> ("IsFolder");
    auto IWF   = unpack.coerce <plist::Boolean> ("IsWrapperFolder");
    auto ISFI  = unpack.coerce <plist::Boolean> ("IsScannedForIncludes");
    auto IFW   = unpack.coerce <plist::Boolean> ("IsFrameworkWrapper");
    auto ISFW  = unpack.coerce <plist::Boolean> ("IsStaticFrameworkWrapper");
    auto IPW   = unpack.coerce <plist::Boolean> ("IsProjectWrapper");
    auto ITW   = unpack.coerce <plist::Boolean> ("IsTargetWrapper");
    auto EPNs  = unpack.cast <plist::Array> ("ExtraPropertyNames");
    auto CPs   = unpack.cast <plist::Dictionary> ("ComponentParts");
    auto III   = unpack.coerce <plist::Boolean> ("IncludeInIndex");
    auto CSIII = unpack.coerce <plist::Boolean> ("CanSetIncludeInIndex");
    auto RHT   = unpack.coerce <plist::Boolean> ("RequiresHardTabs");
    auto CNC   = unpack.coerce <plist::Boolean> ("ContainsNativeCode");
    auto PDS   = unpack.cast <plist::String> ("PlistStructureDefinition");
    auto CCDGI = unpack.coerce <plist::Boolean> ("ChangesCauseDependencyGraphInvalidation");
    auto FABP  = unpack.cast <plist::String> ("FallbackAutoroutingBuildPhase");
    auto CSOC  = unpack.coerce <plist::Boolean> ("CodeSignOnCopy");
    auto RHOC  = unpack.coerce <plist::Boolean> ("RemoveHeadersOnCopy");
    auto VOC   = unpack.coerce <plist::Boolean> ("ValidateOnCopy");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (U != nullptr) {
        _uti = U->value();
    }

    //
    // Extensions are not inherited.
    //
    _extensions.clear();

    if (Es != nullptr) {
        for (size_t n = 0; n < Es->count(); n++) {
            auto E = Es->value <plist::String> (n);
            if (E != nullptr) {
                _extensions.push_back(E->value());
            }
        }
    }

    if (MTs != nullptr) {
        for (size_t n = 0; n < MTs->count(); n++) {
            auto MT = MTs->value <plist::String> (n);
            if (MT != nullptr) {
                _mimeTypes.push_back(MT->value());
            }
        }
    }

    if (TCs != nullptr) {
        for (size_t n = 0; n < TCs->count(); n++) {
            auto TC = TCs->value <plist::String> (n);
            if (TC != nullptr) {
                _typeCodes.push_back(TC->value());
            }
        }
    }

    if (FPs != nullptr) {
        for (size_t n = 0; n < FPs->count(); n++) {
            auto FP = FPs->value <plist::String> (n);
            if (FP != nullptr) {
                _filenamePatterns.push_back(FP->value());
            }
        }
    }

    if (MWs != nullptr) {
        for (size_t n = 0; n < MWs->count(); n++) {
            if (auto MW = MWs->value <plist::String> (n)) {
                std::vector<uint8_t> MWV = std::vector<uint8_t>(MW->value().begin(), MW->value().end());
                _magicWords.push_back(MWV);
            } else if (auto MW = MWs->value <plist::Data> (n)) {
                _magicWords.push_back(MW->value());
            }
        }
    }

    if (L != nullptr) {
        _language = L->value();
    }

    if (CL != nullptr) {
        _computerLanguage = CL->value();
    }

    if (GDN != nullptr) {
        _gccDialectName = GDN->value();
    }

    if (Ps != nullptr) {
        for (size_t n = 0; n < Ps->count(); n++) {
            auto P = Ps->value <plist::String> (n);
            if (P != nullptr) {
                _prefix.push_back(P->value());
            }
        }
    }

    if (P != nullptr) {
        _permissions = P->value();
    }

    if (BPIWE != nullptr) {
        for (size_t n = 0; n < BPIWE->count(); n++) {
            auto BPI = BPIWE->value <plist::Dictionary> (n);
            if (BPI != nullptr) {
                BuildPhaseInjection injection;
                if (injection.parse(BPI)) {
                    _buildPhaseInjectionsWhenEmbedding.push_back(injection);
                }
            }
        }
    }

    if (ATBR != nullptr) {
        _appliesToBuildRules = ATBR->value();
    }

    if (IT != nullptr) {
        _isTransparent = IT->value();
    }

    if (ID != nullptr) {
        _isDocumentation = ID->value();
    }

    if (IBPF != nullptr) {
        _isBuildPropertiesFile = IBPF->value();
    }

    if (ISC != nullptr) {
        _isSourceCode = ISC->value();
    }

    if (IP != nullptr) {
        _isPreprocessed = IP->value();
    }

    if (IT != nullptr) {
        _isTransparent = IT->value();
    }

    if (IEM != nullptr) {
        _isEmbeddable = IEM->value();
    }

    if (IE != nullptr) {
        _isExecutable = IE->value();
    }

    if (IEWG != nullptr) {
        _isExecutableWithGUI = IEWG->value();
    }

    if (IA != nullptr) {
        _isApplication = IA->value();
    }

    if (IB != nullptr) {
        _isBundle = IB->value();
    }

    if (IL != nullptr) {
        _isLibrary = IL->value();
    }

    if (IDL != nullptr) {
        _isDynamicLibrary = IDL->value();
    }

    if (ISL != nullptr) {
        _isStaticLibrary = ISL->value();
    }

    if (IF != nullptr) {
        _isFolder = IF->value();
    }

    if (IWF != nullptr) {
        _isWrappedFolder = IWF->value();
    }

    if (IFW != nullptr) {
        _isFrameworkWrapper = IFW->value();
    }

    if (ISFW != nullptr) {
        _isStaticFrameworkWrapper = ISFW->value();
    }

    if (IPW != nullptr) {
        _isProjectWrapper = IPW->value();
    }

    if (ITW != nullptr) {
        _isTargetWrapper = ITW->value();
    }

    if (EPNs != nullptr) {
        for (size_t n = 0; n < EPNs->count(); n++) {
            auto EPN = EPNs->value <plist::String> (n);
            if (EPN != nullptr) {
                _extraPropertyNames.push_back(EPN->value());
            }
        }
    }

    if (CPs != nullptr) {
        for (size_t n = 0; n < CPs->count(); n++) {
            auto K  = CPs->key(n);
            auto CP = CPs->value <plist::Array> (n);
            if (CP != nullptr) {
                ComponentPart::shared_ptr cp;
                cp.reset(new ComponentPart);
                if (cp->parse(this, CP)) {
                    _componentParts.insert(std::make_pair(K, cp));
                }
            }
        }
    }

    if (ISFI != nullptr) {
        _isScannedForIncludes = ISFI->value();
    }

    if (III != nullptr) {
        _includeInIndex = III->value();
    }

    if (CSIII != nullptr) {
        _canSetIncludeInIndex = CSIII->value();
    }

    if (RHT != nullptr) {
        _requiresHardTabs = RHT->value();
    }

    if (CNC != nullptr) {
        _containsNativeCode = CNC->value();
    }

    if (PDS != nullptr) {
        _plistStructureDefinition = PDS->value();
    }

    if (CCDGI != nullptr) {
        _changesCauseDependencyGraphInvalidation = CCDGI->value();
    }

    if (FABP != nullptr) {
        _fallbackAutoroutingBuildPhase = FABP->value();
    }

    if (CSOC != nullptr) {
        _codeSignOnCopy = CSOC->value();
    }

    if (RHOC != nullptr) {
        _removeHeadersOnCopy = RHOC->value();
    }

    if (VOC != nullptr) {
        _validateOnCopy = VOC->value();
    }

    return true;
}

FileType::ComponentPart::ComponentPart()
{
}

void FileType::ComponentPart::
reset()
{
    _reference.reset();
}

bool FileType::ComponentPart::
parse(FileType *ftype, plist::Array const *array)
{
    auto count = array->count();

    if (count >= 1) {
        if (auto T = array->value <plist::String> (0)) {
            _type = T->value();
        }
    }

    if (count >= 2) {
        if (auto L = array->value <plist::String> (1)) {
            _location = L->value();
        }
    }

    if (count >= 3) {
        if (auto IDs = array->value <plist::Array> (2)) {
            for (size_t m = 0; m < IDs->count(); m++) {
                if (auto ID = IDs->value <plist::String> (m)) {
                    _identifiers.push_back(ID->value());
                }
            }
        } else if (auto R = array->value <plist::String> (2)) {
            _reference = ftype->_componentParts[R->value()];
        }
    }

    return true;
}

FileType::BuildPhaseInjection::BuildPhaseInjection() :
    _runOnlyForDeploymentPostprocessing (false),
    _needsRunpathSearchPathForFrameworks(false),
    _dstSubfolderSpec                   (0),
    _dstPath                            (pbxsetting::Value::Empty())
{
}

bool FileType::BuildPhaseInjection::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("BuildPhaseInjection", dict, &seen);

    auto BP     = unpack.cast <plist::String> ("BuildPhase");
    auto N      = unpack.cast <plist::String> ("Name");
    auto ROFDP  = unpack.coerce <plist::Boolean> ("RunOnlyForDeploymentPostprocessing");
    auto NRSPFF = unpack.coerce <plist::Boolean> ("NeedsRunpathSearchPathForFrameworks");
    auto DSS    = unpack.coerce <plist::Integer> ("DstSubFolderSpec");
    auto DP     = unpack.cast <plist::String> ("DstPath");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

    if (BP != nullptr) {
        _buildPhase = BP->value();
    }

    if (N != nullptr) {
        _name = N->value();
    }

    if (ROFDP != nullptr) {
        _runOnlyForDeploymentPostprocessing = ROFDP->value();
    }

    if (NRSPFF != nullptr) {
        _needsRunpathSearchPathForFrameworks = NRSPFF->value();
    }

    if (DSS != nullptr) {
        _dstSubfolderSpec = DSS->value();
    }

    if (DP != nullptr) {
        _dstPath = pbxsetting::Value::Parse(DP->value());
    }

    return true;
}
