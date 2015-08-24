// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/BuildSystem.h>

using pbxspec::PBX::BuildSystem;

BuildSystem::BuildSystem(bool isDefault) :
    Specification      (ISA::PBXBuildSystem, isDefault)
{
}

BuildSystem::~BuildSystem()
{
}

pbxsetting::Level BuildSystem::
defaultSettings(void) const
{
    std::vector<pbxsetting::Setting> settings;
    std::transform(_properties.begin(), _properties.end(), std::back_inserter(settings), [](PBX::PropertyOption::shared_ptr const &option) -> pbxsetting::Setting {
        return option->defaultSetting();
    });
    std::transform(_options.begin(), _options.end(), std::back_inserter(settings), [](PBX::PropertyOption::shared_ptr const &option) -> pbxsetting::Setting {
        return option->defaultSetting();
    });
    return pbxsetting::Level(settings);
}

BuildSystem::shared_ptr BuildSystem::
Parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    BuildSystem::shared_ptr result;
    auto C = dict->value <plist::String> ("Class");
    if (C == nullptr) {
        result.reset(new BuildSystem(true));
    } else {
        fprintf(stderr, "warning: build system class '%s' not recognized\n",
                C->value().c_str());
        result.reset(new BuildSystem(true));
    }
    if (!result->parse(manager, dict))
        return nullptr;

    return result;
}

bool BuildSystem::
parse(std::shared_ptr<Manager> manager, plist::Dictionary const *dict)
{
    plist::WarnUnhandledKeys(dict, "BuildPhase",
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
        // BuildSystem
        plist::MakeKey <plist::Array> ("Options"),
        plist::MakeKey <plist::Array> ("Properties"));

    if (!Specification::parse(manager, dict))
        return false;

    auto Os = dict->value <plist::Array> ("Options");
    auto Ps = dict->value <plist::Array> ("Properties");

    if (Os != nullptr) {
        for (size_t n = 0; n < Os->count(); n++) {
            if (auto O = Os->value <plist::Dictionary> (n)) {
                PropertyOption::shared_ptr option;
                option.reset(new PropertyOption);
                if (option->parse(O)) {
                    _options.push_back(option);
                }
            }
        }
    }

    if (Ps != nullptr) {
        for (size_t n = 0; n < Ps->count(); n++) {
            if (auto P = Ps->value <plist::Dictionary> (n)) {
                PropertyOption::shared_ptr property;
                property.reset(new PropertyOption);
                if (property->parse(P)) {
                    _properties.push_back(property);
                }
            }
        }
    }

    return true;
}

bool BuildSystem::
inherit(Specification::shared_ptr const &base)
{
    if (!base->isa(BuildSystem::Isa()))
        return false;

    return inherit(reinterpret_cast <BuildSystem::shared_ptr const &> (base));
}

bool BuildSystem::
inherit(BuildSystem::shared_ptr const &b)
{
    if (!Specification::inherit(b))
        return false;

    auto base = this->base();

    _options            = base->options();
    _properties         = base->properties();

    return true;
}
