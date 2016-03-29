/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Slot/WatchIconRole.h>

using xcassets::Slot::WatchIconRole;
using xcassets::Slot::WatchIconRoles;

ext::optional<WatchIconRole> WatchIconRoles::
Parse(std::string const &value)
{
    if (value == "notificationCenter") {
        return WatchIconRole::NotificationCenter;
    } else if (value == "companionSettings") {
        return WatchIconRole::CompanionSettings;
    } else if (value == "appLauncher") {
        return WatchIconRole::AppLauncher;
    } else if (value == "longLook") {
        return WatchIconRole::LongLookNotification;
    } else if (value == "quickLook") {
        return WatchIconRole::ShortLookNotification;
    } else {
        fprintf(stderr, "warning: unknown watch icon role %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string WatchIconRoles::
String(WatchIconRole watchIconRole)
{
    switch (watchIconRole) {
        case WatchIconRole::NotificationCenter:
            return "notificationCenter";
        case WatchIconRole::CompanionSettings:
            return "companionSettings";
        case WatchIconRole::AppLauncher:
            return "appLauncher";
        case WatchIconRole::LongLookNotification:
            return "longLook";
        case WatchIconRole::ShortLookNotification:
            return "quickLook";
    }
}
