/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Slot_WatchIconRole_h
#define __xcassets_Slot_WatchIconRole_h

#include <ext/optional>
#include <string>

namespace xcassets {
namespace Slot {

/*
 * How an icon is used on a watch.
 */
enum class WatchIconRole {
    NotificationCenter,
    CompanionSettings,
    AppLauncher,
    LongLookNotification,
    ShortLookNotification,
};

class WatchIconRoles {
private:
    WatchIconRoles();
    ~WatchIconRoles();

public:
    /*
     * Parse a matching watch icon role from a string, if valid.
     */
    static ext::optional<WatchIconRole> Parse(std::string const &value);

    /*
     * Convert an watch icon role to a string.
     */
    static std::string String(WatchIconRole watchIconRole);
};

}
}

#endif // !__xcassets_Slot_WatchIconRole_h
