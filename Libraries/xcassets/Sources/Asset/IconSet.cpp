/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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

