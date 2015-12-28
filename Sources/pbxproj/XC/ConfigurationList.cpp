/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/XC/ConfigurationList.h>

#include <cassert>

using pbxproj::XC::ConfigurationList;

ConfigurationList::ConfigurationList() :
    Object                        (Isa()),
    _defaultConfigurationIsVisible(false)
{
}

bool ConfigurationList::
parse(Context &context, plist::Dictionary const *dict)
{
    auto DCN  = dict->value <plist::String> ("defaultConfigurationName");
    auto DCIV = dict->value <plist::String> ("defaultConfigurationIsVisible");
    auto BCs  = dict->value <plist::Array> ("buildConfigurations");

    if (DCN != nullptr) {
        _defaultConfigurationName = DCN->value();
    }

    if (DCIV != nullptr) {
        _defaultConfigurationIsVisible = (pbxsetting::Type::ParseInteger(DCIV->value()) != 0);
    }

    if (BCs != nullptr) {
        for (size_t n = 0; n < BCs->count(); n++) {
            std::string BCID;

            auto BCd = context.get <BuildConfiguration> (BCs->value(n), &BCID);
            assert(BCd != nullptr);

            auto BC = context.parseObject(context.buildConfigurations, BCID, BCd);
            if (!BC)
                return false;

            _buildConfigurations.push_back(BC);
        }
    }

    return true;
}
