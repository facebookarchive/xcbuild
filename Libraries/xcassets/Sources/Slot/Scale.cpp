/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Slot/Scale.h>

#include <sstream>

using xcassets::Slot::Scale;

Scale::
Scale(double value) :
    _value(value)
{
}

ext::optional<Scale> Scale::
Parse(std::string const &value)
{
    /* Must end in 'x'. */
    if (value.empty() || value[value.size() - 1] != 'x') {
        fprintf(stderr, "warning: scale not valid %s\n", value.c_str());
        return ext::nullopt;
    }

    /* Number should be the rest. */
    std::string number = value.substr(0, value.size() - 1);

    char *end = NULL;
    double scale = std::strtod(number.c_str(), &end);
    if (end != number.c_str()) {
        return Scale(scale);
    } else {
        fprintf(stderr, "warning: scale not a number %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string Scale::
String(Scale scale)
{
    std::ostringstream out;
    out << scale.value();
    out << "x";
    return out.str();
}
