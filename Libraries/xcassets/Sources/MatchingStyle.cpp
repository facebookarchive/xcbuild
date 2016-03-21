/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/MatchingStyle.h>

using xcassets::MatchingStyle;
using xcassets::MatchingStyles;

ext::optional<MatchingStyle> MatchingStyles::
Parse(std::string const &value)
{
    if (value == "fully-qualified-name") {
        return MatchingStyle::FullyQualifiedName;
    } else {
        fprintf(stderr, "warning: unknown matching style %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string MatchingStyles::
String(MatchingStyle style)
{
    switch (style) {
        case MatchingStyle::FullyQualifiedName:
            return "fully-qualified-name";
    }
}
