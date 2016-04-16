/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcassets/Asset/Catalog.h>
#include <plist/Keys/Unpack.h>
#include <libutil/FSUtil.h>

using xcassets::Asset::Catalog;
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

