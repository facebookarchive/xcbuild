/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Catalog.h>
#include <plist/Keys/Unpack.h>
#include <libutil/FSUtil.h>

using xcassets::Catalog;
using libutil::FSUtil;

bool Catalog::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* No contents is allowed for . */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("Catalog", dict, seen);

        /* No additional contents. */

        if (!unpack.complete(check)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }
    }

    if (!loadChildren(&_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

