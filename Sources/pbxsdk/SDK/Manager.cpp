// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/SDK/Manager.h>

using pbxsdk::SDK::Manager;
using libutil::FSUtil;

Manager::Manager()
{
}

Manager::~Manager()
{
}

Manager::shared_ptr Manager::
Open(std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    Platform::vector platforms;

    std::string platformsPath = path + "/Platforms";
    FSUtil::EnumerateDirectory(platformsPath, "*.platform",
            [&](std::string const &filename) -> bool
            {
                auto platform = SDK::Platform::Open(platformsPath + "/" + filename);
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

    Toolchain::vector toolchains;

    std::string toolchainsPath = path + "/Toolchains";
    FSUtil::EnumerateDirectory(toolchainsPath, "*.xctoolchain",
            [&](std::string const &filename) -> bool
            {
                auto toolchain = SDK::Toolchain::Open(toolchainsPath + "/" + filename);
                if (toolchain) {
                    toolchains.push_back(toolchain);
                }

                return true;
            });

    if (toolchains.empty())
        return nullptr;

    std::sort(toolchains.begin(), toolchains.end(),
            [](Toolchain::shared_ptr const &a, Toolchain::shared_ptr const &b) -> bool
            {
                return (a->identifier() < b->identifier());
            });

    auto manager = std::make_shared <Manager> ();
    manager->_path = path;
    manager->_platforms = platforms;
    manager->_toolchains = toolchains;
    return manager;
}
