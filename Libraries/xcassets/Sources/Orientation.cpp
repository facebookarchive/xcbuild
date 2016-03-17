/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Orientation.h>

using xcassets::Orientation;
using xcassets::Orientations;

ext::optional<Orientation> Orientations::
Parse(std::string const &value)
{
    if (value == "portrait") {
        return Orientation::Portrait;
    } else if (value == "landscape") {
        return Orientation::Landscape;
    } else {
        fprintf(stderr, "warning: unknown orientation %s\n", value.c_str());
        return ext::nullopt;
    }
}

