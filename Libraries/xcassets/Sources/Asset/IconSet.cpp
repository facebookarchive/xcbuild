/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset/IconSet.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::IconSet;

bool IconSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is optional. */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("IconSet", dict, seen);

        /* No properties. */

        if (!unpack.complete(check)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }
    }

    // TODO: confirm no children

    return true;
}

