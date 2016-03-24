/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Slot_Idiom_h
#define __xcassets_Slot_Idiom_h

#include <ext/optional>
#include <string>

namespace xcassets {
namespace Slot {

/*
 * A class of device interaction.
 */
enum class Idiom {
    Universal,
    Phone,
    Pad,
    Desktop,
    TV,
    Watch,
};

class Idioms {
private:
    Idioms();
    ~Idioms();

public:
    /*
     * Parse a matching idiom from a string, if valid.
     */
    static ext::optional<Idiom> Parse(std::string const &value);

    /*
     * Convert an idiom to a string.
     */
    static std::string String(Idiom idiom);
};

}
}

#endif // !__xcassets_Slot_Idiom_h
