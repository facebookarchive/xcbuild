/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Slot/Idiom.h>

#include <cstdlib>

using xcassets::Slot::Idiom;
using xcassets::Slot::Idioms;

ext::optional<Idiom> Idioms::
Parse(std::string const &value)
{
    if (value == "universal") {
        return Idiom::Universal;
    } else if (value == "iphone") {
        return Idiom::Phone;
    } else if (value == "ipad") {
        return Idiom::Pad;
    } else if (value == "mac") {
        return Idiom::Desktop;
    } else if (value == "tv") {
        return Idiom::TV;
    } else if (value == "watch") {
        return Idiom::Watch;
    } else {
        fprintf(stderr, "warning: unknown idiom %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string Idioms::
String(Idiom idiom)
{
    switch (idiom) {
        case Idiom::Universal:
            return "universal";
        case Idiom::Phone:
            return "iphone";
        case Idiom::Pad:
            return "ipad";
        case Idiom::Desktop:
            return "mac";
        case Idiom::TV:
            return "tv";
        case Idiom::Watch:
            return "watch";
    }

    abort();
}
