/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __xcassets_MatchingStyle_h
#define __xcassets_MatchingStyle_h

#include <ext/optional>
#include <string>

namespace xcassets {

/*
 * A method to match an asset by name.
 */
enum class MatchingStyle {
    /*
     * The only supported style. Use the asset name and
     * any groups defining a namespace containing it.
     */
    FullyQualifiedName,
};

class MatchingStyles {
private:
    MatchingStyles();
    ~MatchingStyles();

public:
    /*
     * Parse a matching style from a string, if valid.
     */
    static ext::optional<MatchingStyle> Parse(std::string const &value);

    /*
     * Convert a style to a string.
     */
    static std::string String(MatchingStyle style);
};

}

#endif // !__xcassets_MatchingStyle_h
