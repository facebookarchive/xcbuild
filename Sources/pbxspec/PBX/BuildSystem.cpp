/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxspec/PBX/BuildSystem.h>

using pbxspec::PBX::BuildSystem;

BuildSystem::BuildSystem() :
    Specification()
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

    std::unordered_set<std::string> seen;
    if (!result->parse(context, dict, &seen, true))
        return nullptr;

    return result;
}

bool BuildSystem::
parse(Context *context, plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Specification::parse(context, dict, seen, false))
        return false;

    auto unpack = plist::Keys::Unpack("BuildSystem", dict, seen);

    auto Os = unpack.cast <plist::Array> ("Options");
    auto Ps = unpack.cast <plist::Array> ("Properties");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errors().c_str());
    }

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
    if (base->type() != BuildSystem::Type())
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
