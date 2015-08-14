// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsdk/SDK/Manager.h>

using pbxsdk::SDK::Manager;
using libutil::FSUtil;

bool Manager::
GetPlatforms(std::string const &path, Platform::vector &platforms)
{
    if (path.empty())
        return false;

    platforms.clear();

    std::string basePath = path + "/Platforms";
    FSUtil::EnumerateDirectory(basePath, "*.platform",
            [&](std::string const &filename) -> bool
            {
                auto platform = SDK::Platform::Open(basePath + "/" + filename);
                if (platform) {
                    platforms.push_back(platform);
                }

                return true;
            });

    if (platforms.empty())
        return false;

    std::sort(platforms.begin(), platforms.end(),
            [](Platform::shared_ptr const &a, Platform::shared_ptr const &b) -> bool
            {
                return (a->description() < b->description());
            });

    return true;
}
