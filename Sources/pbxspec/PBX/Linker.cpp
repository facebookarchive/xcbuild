// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/Linker.h>
#include <pbxspec/PBX/LinkerSpecificationLd.h>
#include <pbxspec/PBX/LinkerSpecificationLibtool.h>
#include <pbxspec/PBX/LinkerSpecificationResMerger.h>
#include <pbxspec/Manager.h>

using pbxspec::PBX::Linker;

Linker::Linker(bool isDefault) :
    Linker(isDefault, ISA::PBXLinker)
{
}

Linker::Linker(bool isDefault, std::string const &isa) :
    Tool                  (isDefault, isa),
    _supportsInputFileList(false)
{
}

Linker::~Linker()
{
}

bool Linker::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(Linker::Isa()))
        return false;

    return inherit(reinterpret_cast <Linker::shared_ptr const &> (base));
}

bool Linker::
inherit(Tool::shared_ptr const &base)
{
    if (!base->isa(Linker::Isa()))
        return false;

    return inherit(reinterpret_cast <Linker::shared_ptr const &> (base));
}

bool Linker::
inherit(Linker::shared_ptr const &b)
{
    if (!Tool::inherit(reinterpret_cast <Tool::shared_ptr const &> (b)))
        return false;

    auto base = this->base();

    _binaryFormats         = base->binaryFormats();
    _dependencyInfoFile    = base->dependencyInfoFile();
    _supportsInputFileList = base->supportsInputFileList();

    return true;
}

Linker::shared_ptr Linker::
Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    Linker::shared_ptr result;

    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new Linker(true));
    } else if (C->value() == LinkerSpecificationLd::Isa()) {
        result.reset(new LinkerSpecificationLd(true));
    } else if (C->value() == LinkerSpecificationLibtool::Isa()) {
        result.reset(new LinkerSpecificationLibtool(true));
    } else if (C->value() == LinkerSpecificationResMerger::Isa()) {
        result.reset(new LinkerSpecificationResMerger(true));
    } else {
        fprintf(stderr, "warning: linker class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new Linker(true));
    }

    if (!result->parse(manager, dict))
        return nullptr;

    return result;
}

bool Linker::
parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "Linker",
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
        plist::MakeKey <plist::Boolean> ("DeeplyStatInputDirectories"),
        plist::MakeKey <plist::Array> ("Options"),
        plist::MakeKey <plist::Array> ("DeletedProperties"),
        // Linker
        plist::MakeKey <plist::Array> ("BinaryFormats"),
        plist::MakeKey <plist::String> ("DependencyInfoFile"),
        plist::MakeKey <plist::Boolean> ("SupportsInputFileList"));

    if (!Tool::parse(manager, dict, false))
        return false;

    auto BFs  = dict->value <plist::Array> ("BinaryFormats");
    auto DIF  = dict->value <plist::String> ("DependencyInfoFile");
    auto SIFL = dict->value <plist::Boolean> ("SupportsInputFileList");

    if (BFs != nullptr) {
        for (size_t n = 0; n < BFs->count(); n++) {
            if (auto BF = BFs->value <plist::String> (n)) {
                _binaryFormats.push_back(BF->value());
            }
        }
    }

    if (DIF != nullptr) {
        _dependencyInfoFile = DIF->value();
    }

    if (SIFL != nullptr) {
        _supportsInputFileList = SIFL->value();
    }

    return true;
}
