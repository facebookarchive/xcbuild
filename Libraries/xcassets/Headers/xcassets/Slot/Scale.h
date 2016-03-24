/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Slot_Scale_h
#define __xcassets_Slot_Scale_h

#include <ext/optional>
#include <string>

namespace xcassets {
namespace Slot {

class Scale {
private:
    Scale();
    ~Scale();

public:
    /*
     * Parse a matching scale from a string, if valid.
     */
    static ext::optional<double> Parse(std::string const &value);

    /*
     * Convert an scale to a string.
     */
    static std::string String(double scale);
};

}
}

#endif // !__xcassets_Slot_Scale_h
