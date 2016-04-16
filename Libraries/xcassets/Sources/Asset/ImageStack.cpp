/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcassets/Asset/ImageStack.h>
#include <xcassets/Asset/ImageStackLayer.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::ImageStack;
using xcassets::Asset::ImageStackLayer;

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

