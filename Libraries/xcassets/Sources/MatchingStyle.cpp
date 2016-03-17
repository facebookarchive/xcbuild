/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/MatchingStyle.h>

using xcassets::MatchingStyle;
using xcassets::MatchingStyles;

ext::optional<MatchingStyle> MatchingStyles::
Parse(std::string const &value)
{
    if (value == "fully-qualified-name") {
        return MatchingStyle::FullyQualifiedName;
    }

    // TODO: warn about unknown styles
    return ext::nullopt;
}

