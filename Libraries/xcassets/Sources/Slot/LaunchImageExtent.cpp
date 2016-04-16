/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Slot/LaunchImageExtent.h>

#include <cstdlib>

using xcassets::Slot::LaunchImageExtent;
using xcassets::Slot::LaunchImageExtents;

ext::optional<LaunchImageExtent> LaunchImageExtents::
Parse(std::string const &value)
{
    if (value == "to-status-bar") {
        return LaunchImageExtent::ToStatusBar;
    } else if (value == "regular") {
        return LaunchImageExtent::FullScreen;
    } else {
        fprintf(stderr, "warning: unknown extent '%s'\n", value.c_str());
        return ext::nullopt;
    }
}

std::string LaunchImageExtents::
String(LaunchImageExtent extent)
{
    switch (extent) {
        case LaunchImageExtent::ToStatusBar:
            return "to-status-bar";
        case LaunchImageExtent::FullScreen:
            return "regular";
    }

    abort();
}

