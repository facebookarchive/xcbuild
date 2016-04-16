/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset/GCDashboardImage.h>
#include <xcassets/Asset/ImageSet.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::GCDashboardImage;
using xcassets::Asset::ImageSet;

bool GCDashboardImage::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* No contents is allowed for groups. */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("GCDashboardImage", dict, seen);

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

    if (!loadChildren<ImageSet>(&_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}


