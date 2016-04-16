/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Slot/SizeClass.h>

#include <cstdlib>

using xcassets::Slot::SizeClass;
using xcassets::Slot::SizeClasses;

ext::optional<SizeClass> SizeClasses::
Parse(std::string const &value)
{
    if (value == "compact") {
        return SizeClass::Compact;
    } else if (value == "regular") {
        return SizeClass::Regular;
    } else {
        fprintf(stderr, "warning: unknown size class %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string SizeClasses::
String(SizeClass sizeClass)
{
    switch (sizeClass) {
        case SizeClass::Compact:
            return "compact";
        case SizeClass::Regular:
            return "regular";
    }

    abort();
}
