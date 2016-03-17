/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/ImageStackLayer.h>
#include <plist/Keys/Unpack.h>

using xcassets::ImageStackLayer;

bool ImageStackLayer::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("ImageStackLayer", dict, seen);

    // TODO: content-reference
    // TODO: frame-size
    // TODO: frame-center

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    // TODO: children image set

    return true;
}

