/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcassets/Slot/ImageSize.h>

#include <sstream>

using xcassets::Slot::ImageSize;

ImageSize::
ImageSize(std::vector<double> value) :
    _value(value)
{
}

ext::optional<ImageSize> ImageSize::
Parse(std::string const &value)
{
    /* Must not be empty */
    if (value.empty()) {
        fprintf(stderr, "warning: size not valid %s\n", value.c_str());
        return ext::nullopt;
    }

    std::stringstream sstream(value);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(sstream, segment, 'x'))
    {
       seglist.push_back(segment);
    }

    /* Must contain two strings separated by 'x' */
    if (seglist.size() != 2) {
        fprintf(stderr, "warning: size not valid %s\n", value.c_str());
        return ext::nullopt;
    }

    double w = std::strtod(seglist[0].c_str(), NULL);
    double h = std::strtod(seglist[1].c_str(), NULL);
    std::vector<double> v = {w, h};

    return ImageSize(v);
}

std::string ImageSize::
String(ImageSize scale)
{
    std::ostringstream out;
    auto v = scale.value();
    out << v[0];
    out << "x";
    out << v[1];
    return out.str();
}
