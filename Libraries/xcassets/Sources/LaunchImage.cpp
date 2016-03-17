/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/LaunchImage.h>
#include <plist/Keys/Unpack.h>

using xcassets::LaunchImage;

bool LaunchImage::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("LaunchImage", dict, seen);

    // TODO: data

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    // TODO: confirm no children

    return true;
}

