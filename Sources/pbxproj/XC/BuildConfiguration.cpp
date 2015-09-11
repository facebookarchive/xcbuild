// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/BuildConfiguration.h>

using pbxproj::XC::BuildConfiguration;
using pbxsetting::Level;
using pbxsetting::Setting;

BuildConfiguration::BuildConfiguration() :
    Object        (Isa()),
    _buildSettings(Level({ }))
{
}

BuildConfiguration::~BuildConfiguration()
{
}

bool BuildConfiguration::
parse(Context &context, plist::Dictionary const *dict)
{
    std::string BCRID;

    auto BCR = context.indirect <PBX::FileReference> (dict, "baseConfigurationReference", &BCRID);
    auto BS  = dict->value <plist::Dictionary> ("buildSettings");
    auto N   = dict->value <plist::String> ("name");

    if (BCR != nullptr) {
        _baseConfigurationReference =
          context.parseObject(context.fileReferences, BCRID, BCR);
        if (!_baseConfigurationReference)
            return false;
    }

    if (BS != nullptr) {
        std::vector<Setting> settings;
        for (size_t n = 0; n < BS->count(); n++) {
            auto BSk = BS->key(n);
            auto BSv = BS->value (BSk);
            Setting setting = Setting::Parse(BSk, pbxsetting::Value::FromObject(BSv).raw());
            settings.push_back(setting);
        }
        _buildSettings = Level(settings);
    }

    if (N != nullptr) {
        _name = N->value();
    }

    return true;
}
