/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset/GCLeaderboardSet.h>
#include <xcassets/Asset/ImageStack.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::GCLeaderboardSet;
using xcassets::Asset::ImageStack;

bool GCLeaderboardSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* No contents is allowed for groups. */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("GCLeaderboardSet", dict, seen);

        auto P = unpack.cast <plist::Dictionary> ("properties");

        if (!unpack.complete(check)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }

        if (P != nullptr) {
            std::unordered_set<std::string> seen;
            auto unpack = plist::Keys::Unpack("Properties", P, &seen);

            auto CR = unpack.cast <plist::Dictionary> ("content-reference");

            if (!unpack.complete(true)) {
                fprintf(stderr, "%s", unpack.errorText().c_str());
            }

            if (CR != nullptr) {
                ContentReference contentReference;
                if (contentReference.parse(CR)) {
                    _contentReference = contentReference;
                }
            }
        }
    }

    if (!loadChildren<ImageStack>(&_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

