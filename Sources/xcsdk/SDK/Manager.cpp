// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcsdk/SDK/Manager.h>

using xcsdk::SDK::Manager;
using pbxsetting::Setting;
using pbxsetting::Level;
using libutil::FSUtil;

Manager::Manager()
{
}

Manager::~Manager()
{
}

pbxsetting::Level Manager::
computedSettings(void) const
{
    std::vector<Setting> settings = {
        Setting::Parse("DEVELOPER_DIR", _path),
        Setting::Parse("DEVELOPER_USR_DIR", "$(DEVELOPER_DIR)/usr"),
        Setting::Parse("DEVELOPER_BIN_DIR", "$(DEVELOPER_DIR)/usr/bin"),
        Setting::Parse("DEVELOPER_APPLICATIONS_DIR", "$(DEVELOPER_DIR)/Applications"),
        Setting::Parse("DEVELOPER_FRAMEWORKS_DIR", "$(DEVELOPER_DIR)/Library/Frameworks"),
        Setting::Parse("DEVELOPER_FRAMEWORKS_DIR_QUOTED", "$(DEVELOPER_DIR)/Library/Frameworks"),
        Setting::Parse("DEVELOPER_LIBRARY_DIR", "$(DEVELOPER_DIR)/Library"),
        Setting::Parse("DEVELOPER_TOOLS_DIR", "$(DEVELOPER_DIR)/Tools"),
        Setting::Parse("DEVELOPER_SDK_DIR", "$(DEVELOPER_DIR)/Platforms/MacOSX.platform/Developer/SDKs"), // TODO(grp): Verify.
        Setting::Parse("LEGACY_DEVELOPER_DIR", "$(DEVELOPER_DIR)/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer"), // TODO(grp): Verify.
        Setting::Parse("DERIVED_DATA_DIR", "$(USER_LIBRARY_DIR)/Developer/Xcode/DerivedData"),
    };

    auto tcit = _toolchains.find(Toolchain::DefaultIdentifier());
    if (tcit != _toolchains.end()) {
        settings.push_back(Setting::Parse("DT_TOOLCHAIN_DIR", tcit->second->path()));
    } else {
        settings.push_back(Setting::Parse("DT_TOOLCHAIN_DIR", ""));
    }

    std::string platforms;
    for (Platform::shared_ptr const &platform : _platforms) {
        if (&platform != &_platforms[0]) {
            platforms += " ";
        }
        platforms += platform->name();
    }
    settings.push_back(Setting::Parse("AVAILABLE_PLATFORMS", platforms));

    return Level(settings);
}

std::shared_ptr<Manager> Manager::
Open(std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    auto manager = std::make_shared <Manager> ();
    manager->_path = path;

    std::map<std::string, std::shared_ptr<Toolchain>> toolchains;

    std::string toolchainsPath = path + "/Toolchains";
    FSUtil::EnumerateDirectory(toolchainsPath, "*.xctoolchain",
            [&](std::string const &filename) -> bool
            {
                auto toolchain = SDK::Toolchain::Open(manager, toolchainsPath + "/" + filename);
                if (toolchain) {
                    toolchains.insert({ toolchain->identifier(), toolchain });
                }

                return true;
            });

    if (toolchains.empty())
        return nullptr;

    manager->_toolchains = toolchains;

    std::vector<std::shared_ptr<Platform>> platforms;

    std::string platformsPath = path + "/Platforms";
    FSUtil::EnumerateDirectory(platformsPath, "*.platform",
            [&](std::string const &filename) -> bool
            {
                auto platform = SDK::Platform::Open(manager, platformsPath + "/" + filename);
                if (platform) {
                    platforms.push_back(platform);
                }

                return true;
            });

    if (platforms.empty())
        return nullptr;

    std::sort(platforms.begin(), platforms.end(),
            [](Platform::shared_ptr const &a, Platform::shared_ptr const &b) -> bool
            {
                return (a->description() < b->description());
            });

    manager->_platforms = platforms;

    return manager;
}
