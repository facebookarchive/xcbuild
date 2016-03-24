/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset/GCLeaderboard.h>
#include <xcassets/Asset/ImageStack.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::GCLeaderboard;
using xcassets::Asset::ImageStack;

bool GCLeaderboard::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* No contents is allowed for groups. */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("GCLeaderboard", dict, seen);

        // TODO content-reference

        if (!unpack.complete(check)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }
    }

    if (!loadChildren<ImageStack>(&_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

