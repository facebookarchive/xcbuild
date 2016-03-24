/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Slot_SizeClass_h
#define __xcassets_Slot_SizeClass_h

#include <ext/optional>
#include <string>

namespace xcassets {
namespace Slot {

/*
 * A general class of the available screen space in a dimension.
 */
enum class SizeClass {
    Compact,
    Regular,
};

class SizeClasses {
private:
    SizeClasses();
    ~SizeClasses();

public:
    /*
     * Parse a matching size class from a string, if valid.
     */
    static ext::optional<SizeClass> Parse(std::string const &value);

    /*
     * Convert an size class to a string.
     */
    static std::string String(SizeClass sizeClass);
};

}
}

#endif // !__xcassets_Slot_SizeClass_h
