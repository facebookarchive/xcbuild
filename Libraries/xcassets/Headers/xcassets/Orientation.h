/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_Orientation_h
#define __xcassets_Orientation_h

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * Physical device orientation class.
 */
enum class Orientation {
    Portrait,
    Landscape,
};

class Orientations {
private:
    Orientations();
    ~Orientations();

public:
    /*
     * Parse an orientation from a string, if valid.
     */
    static ext::optional<Orientation>
    Parse(std::string const &value);
};

}

#endif // !__xcassets_Orientation_h
