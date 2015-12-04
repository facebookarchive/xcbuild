/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/SDK/Platform.h>
#include <xcsdk/SDK/Manager.h>

using xcsdk::SDK::Platform;
using pbxsetting::Level;
using pbxsetting::Setting;
using libutil::FSUtil;

Platform::Platform() :
    _defaultDebuggerSettings(nullptr),
    _defaultProperties      (Level({ })),
    _overrideProperties     (Level({ }))
{
}

Platform::~Platform()
{
    if (_defaultDebuggerSettings != nullptr) {
        _defaultDebuggerSettings->release();
    }
}

static bool
StartsWith(std::string const &str, std::string const &prefix)
{
    if (prefix.size() > str.size()) {
        return false;
    }

    return std::equal(prefix.begin(), prefix.end(), str.begin());
}

static bool
EndsWith(std::string const &str, std::string const &suffix)
{
    if (suffix.size() > str.size()) {
        return false;
    }

    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

Level Platform::
settings() const
{
    std::vector<Setting> settings = {
        Setting::Parse("PLATFORM_NAME", _name),
        Setting::Parse("PLATFORM_DISPLAY_NAME", _description),
        Setting::Parse("PLATFORM_DIR", _path),

        Setting::Parse("PLATFORM_DEVELOPER_USR_DIR", "$(PLATFORM_DIR)/Developer/usr"),
        Setting::Parse("PLATFORM_DEVELOPER_BIN_DIR", "$(PLATFORM_DIR)/Developer/usr/bin"),
        Setting::Parse("PLATFORM_DEVELOPER_APPLICATIONS_DIR", "$(PLATFORM_DIR)/Developer/Applications"),
        Setting::Parse("PLATFORM_DEVELOPER_LIBRARY_DIR", "$(DEVELOPER_DIR)/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library"), // TODO(grp): Verify.
        Setting::Parse("PLATFORM_DEVELOPER_SDK_DIR", "$(PLATFORM_DIR)/Developer/SDKs"),
        Setting::Parse("PLATFORM_DEVELOPER_TOOLS_DIR", "$(PLATFORM_DIR)/Developer/Tools"),
        Setting::Parse("PLATFORM_PRODUCT_BUILD_VERSION", _platformVersion ? _platformVersion->buildVersion() : ""),

        // TODO(grp): PLATFORM_PREFERRED_ARCH

        // TODO(grp): CORRESPONDING_DEVICE_PLATFORM_NAME
        // TODO(grp): CORRESPONDING_DEVICE_PLATFORM_DIR
        // TODO(grp): CORRESPONDING_SIMULATOR_PLATFORM_NAME
        // TODO(grp): CORRESPONDING_SIMULATOR_PLATFORM_DIR
    };

    std::string flagName;
    std::string settingName;
    std::string envName;

    if (StartsWith(_name, "macosx")) {
        flagName = "macosx";
        settingName = "MACOSX";
        envName = "MACOSX";
    } else if (StartsWith(_name, "iphone")) {
        flagName = "ios";
        settingName = "IPHONEOS";
        envName = "IPHONEOS";
    } else if (StartsWith(_name, "appletv")) {
        flagName = "tvos";
        settingName = "TVOS";
        envName = "TVOS";
    } else if (StartsWith(_name, "watch")) {
        flagName = "watchos";
        settingName = "WATCHOS";
        envName = "WATCHOS";
    } else {
        flagName = _name;

        settingName = _name;
        std::transform(settingName.begin(), settingName.end(), settingName.begin(), ::toupper);

        envName = _name;
        std::transform(envName.begin(), envName.end(), envName.begin(), ::toupper);
    }

    bool simulator = EndsWith(_name, "simulator");
    if (simulator) {
        flagName += "-simulator";
    }

    settings.push_back(Setting::Parse("DEPLOYMENT_TARGET_SETTING_NAME", settingName + "_DEPLOYMENT_TARGET"));
    settings.push_back(Setting::Parse("DEPLOYMENT_TARGET_CLANG_FLAG_NAME", "m" + flagName + "-version-min"));
    settings.push_back(Setting::Parse("DEPLOYMENT_TARGET_CLANG_FLAG_PREFIX", "-m" + flagName + "-version-min="));
    settings.push_back(Setting::Parse("DEPLOYMENT_TARGET_CLANG_FLAG_ENV", envName + "_DEPLOYMENT_TARGET"));
    settings.push_back(Setting::Parse("SWIFT_PLATFORM_TARGET_PREFIX", flagName));

    settings.push_back(Setting::Parse("EFFECTIVE_PLATFORM_NAME", _identifier == "com.apple.platform.macosx" ? "" : "-$(PLATFORM_NAME)"));

    std::shared_ptr<Manager> manager = _manager.lock();
    if (manager && !_familyIdentifier.empty()) {
        std::string platforms;
        for (Platform::shared_ptr const &platform : manager->platforms()) {
            if (platform->familyIdentifier() == _familyIdentifier) {
                if (&platform != &manager->platforms()[0]) {
                    platforms += " ";
                }
                platforms += platform->name();
            }
        }

        settings.push_back(Setting::Parse("SUPPORTED_PLATFORMS", platforms));
    } else {
        settings.push_back(Setting::Parse("SUPPORTED_PLATFORMS", _name));
    }

    return Level(settings);
}

std::vector<std::string> Platform::
executablePaths() const
{
    return { _path + "/Developer/usr/bin" };
}

bool Platform::
parse(plist::Dictionary const *dict)
{
    auto I   = dict->value <plist::String> ("Identifier");
    auto N   = dict->value <plist::String> ("Name");
    auto D   = dict->value <plist::String> ("Description");
    auto T   = dict->value <plist::String> ("Type");
    auto V   = dict->value <plist::String> ("Version");
    auto FI  = dict->value <plist::String> ("FamilyIdentifier");
    auto FN  = dict->value <plist::String> ("FamilyName");
    auto Ic  = dict->value <plist::String> ("Icon");
    auto DDS = dict->value <plist::Dictionary> ("DefaultDebuggerSettings");
    auto DP  = dict->value <plist::Dictionary> ("DefaultProperties");
    auto OP  = dict->value <plist::Dictionary> ("OverrideProperties");

    if (I != nullptr) {
        _identifier = I->value();
    }

    if (N != nullptr) {
        _name = N->value();
    }

    if (D != nullptr) {
        _description = D->value();
    }

    if (T != nullptr) {
        _type = T->value();
    }

    if (V != nullptr) {
        _version = V->value();
    }

    if (FI != nullptr) {
        _familyIdentifier = FI->value();
    }

    if (FN != nullptr) {
        _familyName = FN->value();
    }

    if (Ic != nullptr) {
        _icon = Ic->value();
    }

    if (DDS != nullptr) {
        _defaultDebuggerSettings = DDS->copy().release();
    }

    if (DP != nullptr) {
        std::vector<Setting> settings;
        for (size_t n = 0; n < DP->count(); n++) {
            auto DPK = DP->key(n);
            auto DPV = DP->value <plist::String> (DPK);

            if (DPV != nullptr) {
                Setting setting = Setting::Parse(DPK, DPV->value());
                settings.push_back(setting);
            }
        }
        _defaultProperties = Level(settings);
    }

    if (OP != nullptr) {
        std::vector<Setting> settings;
        for (size_t n = 0; n < OP->count(); n++) {
            auto OPK = OP->key(n);
            auto OPV = OP->value <plist::String> (OPK);

            if (OPV != nullptr) {
                Setting setting = Setting::Parse(OPK, OPV->value());
                settings.push_back(setting);
            }
        }
        _overrideProperties = Level(settings);
    }

    return true;
}

Platform::shared_ptr Platform::
Open(std::shared_ptr<Manager> manager, std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    std::string settingsFileName = path + "/Info.plist";
    if (!FSUtil::TestForRead(settingsFileName.c_str()))
        return nullptr;

    std::string realPath = FSUtil::ResolvePath(settingsFileName);
    if (realPath.empty())
        return nullptr;

    //
    // Parse property list
    //
    auto result = plist::Format::Any::Read(settingsFileName);
    if (result.first == nullptr) {
        return nullptr;
    }

    plist::Dictionary *plist = plist::CastTo<plist::Dictionary>(result.first.get());
    if (plist == nullptr) {
        return nullptr;
    }

    //
    // Parse the SDK platform dictionary and create the object.
    //
    auto platform = std::make_shared <Platform> ();
    platform->_manager = manager;

    if (platform->parse(plist)) {
        //
        // Save some useful info
        //
        platform->_path = FSUtil::GetDirectoryName(realPath);
    } else {
        platform = nullptr;
    }

    if (platform) {
        //
        // Parse version information
        //
        platform->_platformVersion = PlatformVersion::Open(platform->_path);

        //
        // Lookup all the SDKs inside the platform
        //
        std::string sdksPath = platform->_path + "/Developer/SDKs";
        FSUtil::EnumerateDirectory(sdksPath, "*.sdk",
                [&](std::string const &filename) -> bool
                {
                    if (auto target = Target::Open(manager, platform, sdksPath + "/" + filename)) {
                        platform->_targets.push_back(target);
                    }
                    return true;
                });

        std::sort(platform->_targets.begin(), platform->_targets.end(),
                [](Target::shared_ptr const &a, Target::shared_ptr const &b) -> bool
                {
                    return (a->canonicalName() < b->canonicalName());
                });
    }

    return platform;
}
