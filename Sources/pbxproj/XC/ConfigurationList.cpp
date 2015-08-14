// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/ConfigurationList.h>

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
    auto DCIV = dict->value <plist::Integer> ("defaultConfigurationIsVisible");
    auto BCs  = dict->value <plist::Array> ("buildConfigurations");

    if (DCN != nullptr) {
        _defaultConfigurationName = DCN->value();
    }
    
    if (DCIV != nullptr) {
        _defaultConfigurationIsVisible = (DCIV->value() != 0);
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
