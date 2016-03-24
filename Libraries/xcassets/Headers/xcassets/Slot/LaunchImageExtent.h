/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Slot_LaunchImageExtent_h
#define __xcassets_Slot_LaunchImageExtent_h

#include <ext/optional>
#include <string>

namespace xcassets {
namespace Slot {

/*
 * The visual extent of a launch image.
 */
enum class LaunchImageExtent {
    ToStatusBar,
    FullScreen,
};

class LaunchImageExtents {
private:
    LaunchImageExtents();
    ~LaunchImageExtents();

public:
    /*
     * Parse an extent from a string.
     */
    static ext::optional<LaunchImageExtent> Parse(std::string const &value);

    /*
     * Convert an extent to a string.
     */
    static std::string String(LaunchImageExtent extent);
};

}
}

#endif // !__xcassets_Slot_LaunchImageExtent_h
