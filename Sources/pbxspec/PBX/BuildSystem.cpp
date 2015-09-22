// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxspec/PBX/BuildSystem.h>

using pbxspec::PBX::BuildSystem;

BuildSystem::BuildSystem() :
    Specification      (ISA::PBXBuildSystem)
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
Parse(Context *context, plist::Dictionary const *dict)
{
    auto T = dict->value <plist::String> ("Type");
    if (T == nullptr || T->value() != Type())
        return nullptr;

    BuildSystem::shared_ptr result;
    result.reset(new BuildSystem());

    if (!result->parse(context, dict))
        return nullptr;

    return result;
}

bool BuildSystem::
parse(Context *context, plist::Dictionary const *dict)
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

    if (!Specification::parse(context, dict))
        return false;

    auto Os = dict->value <plist::Array> ("Options");
    auto Ps = dict->value <plist::Array> ("Properties");

    if (Os != nullptr) {
        for (size_t n = 0; n < Os->count(); n++) {
            if (auto O = Os->value <plist::Dictionary> (n)) {
                PropertyOption::shared_ptr option;
                option.reset(new PropertyOption);
                if (option->parse(O)) {
                    PropertyOption::Insert(&_options, &_optionsUsed, option);
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
                    PropertyOption::Insert(&_properties, &_propertiesUsed, property);
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
    _optionsUsed        = base->_optionsUsed;
    _properties         = base->properties();
    _propertiesUsed     = base->_propertiesUsed;

    return true;
}
