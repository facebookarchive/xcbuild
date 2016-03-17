/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/AppIconSet.h>
#include <plist/Keys/Unpack.h>

using xcassets::AppIconSet;

bool AppIconSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("AppIconSet", dict, seen);

    // TODO: pre-rendered
    // TODO: images

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    // TODO: confirm no children

    return true;
}


