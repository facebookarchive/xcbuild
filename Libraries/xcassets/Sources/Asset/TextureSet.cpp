/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcassets/Asset/TextureSet.h>
#include <xcassets/Asset/MipmapSet.h>
#include <libutil/Filesystem.h>
#include <plist/Array.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::TextureSet;
using xcassets::Asset::MipmapSet;
using libutil::Filesystem;

bool TextureSet::
load(Filesystem const *filesystem)
{
    if (!Asset::load(filesystem)) {
        return false;
    }

    if (!loadChildren<MipmapSet>(filesystem, &_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

bool TextureSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("TextureSet", dict, seen);

    auto P = unpack.cast <plist::Dictionary> ("properties");
    // TODO: textures

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (P != nullptr) {
        std::unordered_set<std::string> seen;
        auto unpack = plist::Keys::Unpack("Properties", P, &seen);

        auto I    = unpack.cast <plist::String> ("interpretation");
        auto O    = unpack.cast <plist::String> ("origin");
        auto ODRT = unpack.cast <plist::Array> ("on-demand-resource-tags");

        if (!unpack.complete(true)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }

        if (I != nullptr) {
            _interpretation = TextureInterpretations::Parse(I->value());
        }

        if (O != nullptr) {
            _origin = TextureOrigins::Parse(O->value());
        }

        if (ODRT != nullptr) {
            _onDemandResourceTags = std::vector<std::string>();
            _onDemandResourceTags->reserve(ODRT->count());

            for (size_t n = 0; n < ODRT->count(); n++) {
                if (auto string = ODRT->value<plist::String>(n)) {
                    _onDemandResourceTags->push_back(string->value());
                }
            }
        }
    }

    // TODO: textures

    return true;
}
