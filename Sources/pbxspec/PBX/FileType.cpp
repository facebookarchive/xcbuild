// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/FileType.h>
#include <pbxspec/PBX/PlistFileType.h>
#include <pbxspec/PBX/HTMLFileType.h>
#include <pbxspec/PBX/MachOFileType.h>
#include <pbxspec/PBX/ApplicationWrapperFileType.h>
#include <pbxspec/PBX/CFBundleWrapperFileType.h>
#include <pbxspec/PBX/FrameworkWrapperFileType.h>
#include <pbxspec/PBX/PlugInKitPluginWrapperFileType.h>
#include <pbxspec/PBX/SpotlightImporternWrapperFileType.h>
#include <pbxspec/PBX/XPCServiceWrapperFileType.h>
#include <pbxspec/XC/StaticFrameworkWrapperFileType.h>
#include <pbxspec/Manager.h>

using pbxspec::PBX::FileType;

FileType::FileType(bool isDefault) :
    FileType(isDefault, ISA::PBXFileType)
{
}

FileType::FileType(bool isDefault, std::string const &isa) :
    Specification                           (isa, isDefault),
    _isTextFile                             (false),
    _isBuildPropertiesFile                  (false),
    _isSourceCode                           (false),
    _isPreprocessed                         (false),
    _isTransparent                          (false),
    _isDocumentation                        (false),
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
    _changesCauseDependencyGraphInvalidation(false)
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
    if (!base->isa(FileType::Isa()))
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
    _isTextFile                              = base->isTextFile();
    _isBuildPropertiesFile                   = base->isBuildPropertiesFile();
    _isSourceCode                            = base->isSourceCode();
    _isDocumentation                         = base->isDocumentation();
    _isPreprocessed                          = base->isPreprocessed();
    _isTransparent                           = base->isTransparent();
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

    return true;
}

FileType::shared_ptr FileType::
Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    FileType::shared_ptr result;

    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new FileType(true));
    } else if (C->value() == PlistFileType::Isa()) {
        result.reset(new PlistFileType(true));
    } else if (C->value() == HTMLFileType::Isa()) {
        result.reset(new HTMLFileType(true));
    } else if (C->value() == MachOFileType::Isa()) {
        result.reset(new MachOFileType(true));
    } else if (C->value() == ApplicationWrapperFileType::Isa()) {
        result.reset(new ApplicationWrapperFileType(true));
    } else if (C->value() == CFBundleWrapperFileType::Isa()) {
        result.reset(new CFBundleWrapperFileType(true));
    } else if (C->value() == FrameworkWrapperFileType::Isa()) {
        result.reset(new FrameworkWrapperFileType(true));
    } else if (C->value() == PlugInKitPluginWrapperFileType::Isa()) {
        result.reset(new PlugInKitPluginWrapperFileType(true));
    } else if (C->value() == SpotlightImporternWrapperFileType::Isa()) {
        result.reset(new SpotlightImporternWrapperFileType(true));
    } else if (C->value() == XPCServiceWrapperFileType::Isa()) {
        result.reset(new XPCServiceWrapperFileType(true));
    } else if (C->value() == XC::StaticFrameworkWrapperFileType::Isa()) {
        result.reset(new XC::StaticFrameworkWrapperFileType(true));
    } else {
        fprintf(stderr, "warning: file type class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new FileType(true));
    }

    if (!result->parse(manager, dict))
        return nullptr;

    return result;
}

bool FileType::
parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "FileType",
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
            // FileType
            plist::MakeKey <plist::Array> ("MIMETypes"),
            plist::MakeKey <plist::String> ("UTI"),
            plist::MakeKey <plist::Array> ("Extensions"),
            plist::MakeKey <plist::Array> ("TypeCodes"),
            plist::MakeKey <plist::Array> ("FilenamePatterns"),
            plist::MakeKey <plist::Array> ("MagicWord"),
            plist::MakeKey <plist::String> ("Language"),
            plist::MakeKey <plist::String> ("ComputerLanguage"),
            plist::MakeKey <plist::String> ("GccDialectName"),
            plist::MakeKey <plist::Array> ("Prefix"),
            plist::MakeKey <plist::String> ("Permissions"),
            plist::MakeKey <plist::Boolean> ("AppliesToBuildRules"),
            plist::MakeKey <plist::Boolean> ("IsTextFile"),
            plist::MakeKey <plist::Boolean> ("IsBuildPropertiesFile"),
            plist::MakeKey <plist::Boolean> ("IsSourceCode"),
            plist::MakeKey <plist::Boolean> ("IsPreprocessed"),
            plist::MakeKey <plist::Boolean> ("IsTransparent"),
            plist::MakeKey <plist::Boolean> ("IsDocumentation"),
            plist::MakeKey <plist::Boolean> ("IsEmbeddable"),
            plist::MakeKey <plist::Boolean> ("IsExecutable"),
            plist::MakeKey <plist::Boolean> ("IsExecutableWithGUI"),
            plist::MakeKey <plist::Boolean> ("IsApplication"),
            plist::MakeKey <plist::Boolean> ("IsBundle"),
            plist::MakeKey <plist::Boolean> ("IsLibrary"),
            plist::MakeKey <plist::Boolean> ("IsDynamicLibrary"),
            plist::MakeKey <plist::Boolean> ("IsStaticLibrary"),
            plist::MakeKey <plist::Boolean> ("IsFolder"),
            plist::MakeKey <plist::Boolean> ("IsWrapperFolder"),
            plist::MakeKey <plist::Boolean> ("IsScannedForIncludes"),
            plist::MakeKey <plist::Boolean> ("IsFrameworkWrapper"),
            plist::MakeKey <plist::Boolean> ("IsStaticFrameworkWrapper"),
            plist::MakeKey <plist::Boolean> ("IsProjectWrapper"),
            plist::MakeKey <plist::Boolean> ("IsTargetWrapper"),
            plist::MakeKey <plist::Array> ("ExtraPropertyNames"),
            plist::MakeKey <plist::Dictionary> ("ComponentParts"),
            plist::MakeKey <plist::Boolean> ("IncludeInIndex"),
            plist::MakeKey <plist::Boolean> ("CanSetIncludeInIndex"),
            plist::MakeKey <plist::Boolean> ("RequiresHardTabs"),
            plist::MakeKey <plist::Boolean> ("ContainsNativeCode"),
            plist::MakeKey <plist::String> ("PlistStructureDefinition"),
            plist::MakeKey <plist::Boolean> ("ChangesCauseDependencyGraphInvalidation"),
            plist::MakeKey <plist::String> ("FallbackAutoroutingBuildPhase"),
            plist::MakeKey <plist::Boolean> ("CodeSignOnCopy"),
            plist::MakeKey <plist::Boolean> ("RemoveHeadersOnCopy"),
            plist::MakeKey <plist::Boolean> ("ValidateOnCopy"));

    if (!Specification::parse(manager, dict))
        return false;

    auto MTs   = dict->value <plist::Array> ("MIMETypes");
    auto U     = dict->value <plist::String> ("UTI");
    auto Es    = dict->value <plist::Array> ("Extensions");
    auto TCs   = dict->value <plist::Array> ("TypeCodes");
    auto FPs   = dict->value <plist::Array> ("FilenamePatterns");
    auto MWs   = dict->value <plist::Array> ("MagicWord");
    auto L     = dict->value <plist::String> ("Language");
    auto CL    = dict->value <plist::String> ("ComputerLanguage");
    auto GDN   = dict->value <plist::String> ("GccDialectName");
    auto Ps    = dict->value <plist::Array> ("Prefix");
    auto P     = dict->value <plist::String> ("Permissions");
    auto ATBR  = dict->value <plist::Boolean> ("AppliesToBuildRules");
    auto ITF   = dict->value <plist::Boolean> ("IsTextFile");
    auto IBPF  = dict->value <plist::Boolean> ("IsBuildPropertiesFile");
    auto ISC   = dict->value <plist::Boolean> ("IsSourceCode");
    auto IP    = dict->value <plist::Boolean> ("IsPreprocessed");
    auto IT    = dict->value <plist::Boolean> ("IsTransparent");
    auto ID    = dict->value <plist::Boolean> ("IsDocumentation");
    auto IEM   = dict->value <plist::Boolean> ("IsEmbeddable");
    auto IE    = dict->value <plist::Boolean> ("IsExecutable");
    auto IEWG  = dict->value <plist::Boolean> ("IsExecutableWithGUI");
    auto IA    = dict->value <plist::Boolean> ("IsApplication");
    auto IB    = dict->value <plist::Boolean> ("IsBundle");
    auto IL    = dict->value <plist::Boolean> ("IsLibrary");
    auto IDL   = dict->value <plist::Boolean> ("IsDynamicLibrary");
    auto ISL   = dict->value <plist::Boolean> ("IsStaticLibrary");
    auto IF    = dict->value <plist::Boolean> ("IsFolder");
    auto IWF   = dict->value <plist::Boolean> ("IsWrapperFolder");
    auto ISFI  = dict->value <plist::Boolean> ("IsScannedForIncludes");
    auto IFW   = dict->value <plist::Boolean> ("IsFrameworkWrapper");
    auto ISFW  = dict->value <plist::Boolean> ("IsStaticFrameworkWrapper");
    auto IPW   = dict->value <plist::Boolean> ("IsProjectWrapper");
    auto ITW   = dict->value <plist::Boolean> ("IsTargetWrapper");
    auto EPNs  = dict->value <plist::Array> ("ExtraPropertyNames");
    auto CPs   = dict->value <plist::Dictionary> ("ComponentParts");
    auto III   = dict->value <plist::Boolean> ("IncludeInIndex");
    auto CSIII = dict->value <plist::Boolean> ("CanSetIncludeInIndex");
    auto RHT   = dict->value <plist::Boolean> ("RequiresHardTabs");
    auto CNC   = dict->value <plist::Boolean> ("ContainsNativeCode");
    auto PDS   = dict->value <plist::String> ("PlistStructureDefinition");
    auto CCDGI = dict->value <plist::Boolean> ("ChangesCauseDependencyGraphInvalidation");
    auto FABP  = dict->value <plist::String> ("FallbackAutoroutingBuildPhase");
    auto CSOC  = dict->value <plist::Boolean> ("CodeSignOnCopy");
    auto RHOC  = dict->value <plist::Boolean> ("RemoveHeadersOnCopy");
    auto VOC   = dict->value <plist::Boolean> ("ValidateOnCopy");

    if (U != nullptr) {
        _uti = U->value();
    }

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
            auto MW = MWs->value <plist::String> (n);
            if (MW != nullptr) {
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
