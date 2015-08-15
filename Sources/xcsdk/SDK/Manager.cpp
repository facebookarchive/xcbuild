// Copyright 2013-present Facebook. All Rights Reserved.

#include <xcsdk/SDK/Manager.h>

using xcsdk::SDK::Manager;
using libutil::FSUtil;

Manager::Manager()
{
}

Manager::~Manager()
{
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
