/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/ImageStack.h>
#include <xcassets/ImageStackLayer.h>
#include <plist/Keys/Unpack.h>

using xcassets::ImageStack;
using xcassets::ImageStackLayer;

bool ImageStack::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("ImageStack", dict, seen);

    // TODO: layers
    // TODO: canvasSize

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (!loadChildren<ImageStackLayer>(&_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

