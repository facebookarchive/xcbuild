/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/GCLeaderboardSet.h>
#include <xcassets/ImageStack.h>
#include <plist/Keys/Unpack.h>

using xcassets::GCLeaderboardSet;
using xcassets::ImageStack;

bool GCLeaderboardSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* No contents is allowed for groups. */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("GCLeaderboardSet", dict, seen);

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

