/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Slot_Scale_h
#define __xcassets_Slot_Scale_h

#include <ext/optional>
#include <string>

namespace xcassets {
namespace Slot {

/*
 * The density ratio of a target device.
 */
class Scale {
private:
    double _value;

private:
    Scale(double scale);

public:
    /*
     * The represented device scale.
     */
    double value() const
    { return _value; }

public:
    /*
     * Parse a matching scale from a string, if valid.
     */
    static ext::optional<Scale> Parse(std::string const &value);

    /*
     * Convert an scale to a string.
     */
    static std::string String(Scale scale);
};

}
}

#endif // !__xcassets_Slot_Scale_h
