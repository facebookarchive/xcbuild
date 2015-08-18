// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/BuildPhase.h>

using pbxspec::PBX::BuildPhase;

BuildPhase::BuildPhase(bool isDefault) :
    Specification(ISA::PBXBuildPhase, isDefault)
{
}

BuildPhase::~BuildPhase()
{
}

BuildPhase::shared_ptr BuildPhase::
Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    BuildPhase::shared_ptr result;
    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new BuildPhase(true));
    } else {
        fprintf(stderr, "warning: build phase class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new BuildPhase(true));
    }
    if (!result->parse(manager, dict))
        return nullptr;

    return result;
}

bool BuildPhase::
parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildPhase",
        // Specification
        plist::MakeKey <plist::String> ("Class"),
        plist::MakeKey <plist::String> ("Type"),
        plist::MakeKey <plist::String> ("Identifier"),
        plist::MakeKey <plist::String> ("BasedOn"),
        plist::MakeKey <plist::String> ("Name"),
        plist::MakeKey <plist::String> ("Description"),
        plist::MakeKey <plist::String> ("Vendor"),
        plist::MakeKey <plist::String> ("Version"),
        // BuildPhase
        plist::MakeKey <plist::Array> ("BuildRules"));

    if (!Specification::parse(manager, dict))
        return false;

    auto BRs = dict->value <plist::Array> ("BuildRules");

    if (BRs != nullptr) {
        for (size_t n = 0; n < BRs->count(); n++) {
            if (auto BR = BRs->value <plist::Dictionary> (n)) {
                BuildRule::shared_ptr buildRule;
                buildRule.reset(new BuildRule);
                if (buildRule->parse(BR)) {
                    _buildRules.push_back(buildRule);
                }
            }
        }
    }

    return true;
}

bool BuildPhase::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(BuildPhase::Isa()))
        return false;

    return inherit(reinterpret_cast <BuildPhase::shared_ptr const &> (base));
}

bool BuildPhase::
inherit(BuildPhase::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _buildRules = base->buildRules();

    return true;
}

BuildPhase::BuildRule::BuildRule()
{
}

bool BuildPhase::BuildRule::
parse(plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildRule",
        plist::MakeKey <plist::Object> ("Name"),
        plist::MakeKey <plist::String> ("FileType"),
        plist::MakeKey <plist::String> ("CompilerSpec")
        );

    auto N  = dict->value <plist::String> ("Name");
    auto FT = dict->value <plist::String> ("FileType");
    auto CS = dict->value <plist::String> ("CompilerSpec");

    if (N != nullptr) {
        _name = N->value();
    }

    if (FT != nullptr) {
        _fileType = FT->value();
    }

    if (CS != nullptr) {
        _compilerSpec = CS->value();
    }

    return true;
}
