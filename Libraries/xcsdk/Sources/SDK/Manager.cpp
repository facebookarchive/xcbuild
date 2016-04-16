/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/SDK/Manager.h>
#include <libutil/FSUtil.h>

using xcsdk::SDK::Manager;
using xcsdk::SDK::Target;
using xcsdk::SDK::Toolchain;
using pbxsetting::Setting;
using pbxsetting::Level;
using libutil::FSUtil;

Manager::Manager()
{
}

Manager::~Manager()
{
}

Target::shared_ptr Manager::
findTarget(std::string const &name) const
{
    for (Platform::shared_ptr const &platform : _platforms) {
        for (Target::shared_ptr const &target : platform->targets()) {
            /* Try both the name and the path; either are valid. */
            if (target->canonicalName() == name || target->path() == name) {
                return target;
            }
        }

        /* If the platform name matches but no targets do, use any target. */
        if (platform->name() == name || platform->path() == name) {
            return platform->targets().back();
        }
    }

    return nullptr;
}

Toolchain::shared_ptr Manager::
findToolchain(std::string const &name) const
{
    for (Toolchain::shared_ptr const &toolchain : _toolchains) {
        /* Match liberally: name, identifier, or path; all are valid. */
        if (toolchain->name() == name || toolchain->identifier() == name || toolchain->path() == name) {
            return toolchain;
        }
    }

    return nullptr;
}

pbxsetting::Level Manager::
computedSettings(void) const
{
    std::vector<Setting> settings = {
        Setting::Create("DEVELOPER_DIR", _path),
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

    if (Toolchain::shared_ptr defaultToolchain = findToolchain(Toolchain::DefaultIdentifier())) {
        settings.push_back(Setting::Create("DT_TOOLCHAIN_DIR", defaultToolchain->path()));
    } else {
        settings.push_back(Setting::Create("DT_TOOLCHAIN_DIR", ""));
    }

    std::vector<std::string> platformNames;
    for (Platform::shared_ptr const &platform : _platforms) {
        platformNames.push_back(platform->name());
    }
    settings.push_back(Setting::Create("AVAILABLE_PLATFORMS", pbxsetting::Type::FormatList(platformNames)));

    return Level(settings);
}

std::vector<std::string> Manager::
executablePaths() const
{
    return {
        _path + "/usr/bin",
        _path + "/Tools",
    };
}

std::shared_ptr<Manager> Manager::
Open(std::string const &path)
{
    if (path.empty()) {
        fprintf(stderr, "error: empty path for sdk manager\n");
        return nullptr;
    }

    auto manager = std::make_shared <Manager> ();
    manager->_path = path;

    std::vector<std::shared_ptr<Toolchain>> toolchains;

    std::string toolchainsPath = path + "/Toolchains";
    FSUtil::EnumerateDirectory(toolchainsPath, [&](std::string const &filename) -> bool {
        if (FSUtil::GetFileExtension(filename) != "xctoolchain") {
            return true;
        }

        auto toolchain = SDK::Toolchain::Open(manager, toolchainsPath + "/" + filename);
        if (toolchain != nullptr) {
            toolchains.push_back(toolchain);
        }

        return true;
    });

    manager->_toolchains = toolchains;

    std::vector<std::shared_ptr<Platform>> platforms;

    std::string platformsPath = path + "/Platforms";
    FSUtil::EnumerateDirectory(platformsPath, [&](std::string const &filename) -> bool {
        if (FSUtil::GetFileExtension(filename) != "platform") {
            return true;
        }

        auto platform = SDK::Platform::Open(manager, platformsPath + "/" + filename);
        if (platform != nullptr) {
            platforms.push_back(platform);
        }

        return true;
    });

    std::sort(platforms.begin(), platforms.end(), [](Platform::shared_ptr const &a, Platform::shared_ptr const &b) -> bool {
        return (a->description() < b->description());
    });

    manager->_platforms = platforms;

    return manager;
}
