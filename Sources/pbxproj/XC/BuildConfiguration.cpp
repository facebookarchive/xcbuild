// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/BuildConfiguration.h>

using pbxproj::XC::BuildConfiguration;

BuildConfiguration::BuildConfiguration() :
    Object        (Isa()),
    _buildSettings(nullptr)
{
}

BuildConfiguration::~BuildConfiguration()
{
    if (_buildSettings != nullptr) {
        _buildSettings->release();
    }
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
        _buildSettings = BS->copy();
    }

    if (N != nullptr) {
        _name = N->value();
    }

    return true;
}
