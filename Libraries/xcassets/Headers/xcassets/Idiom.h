/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Idiom_h
#define __xcassets_Idiom_h

#include <ext/optional>
#include <string>

namespace xcassets {

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
    static ext::optional<Idiom>
    Parse(std::string const &value);
};

}

#endif // !__xcassets_Idiom_h
