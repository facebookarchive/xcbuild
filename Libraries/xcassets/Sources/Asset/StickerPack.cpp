/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcassets/Asset/StickerPack.h>
#include <plist/Array.h>
#include <plist/String.h>
#include <plist/Keys/Unpack.h>

#include <cstdlib>

using xcassets::Asset::StickerPack;
using GridSize = StickerPack::GridSize;
using GridSizes = StickerPack::GridSizes;
using libutil::Filesystem;

bool StickerPack::
load(Filesystem const *filesystem)
{
    if (!Asset::load(filesystem)) {
        return false;
    }

    if (!loadChildren(filesystem, &_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

bool StickerPack::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("StickerPack", dict, seen);

    // TODO: stickers
    auto P = unpack.cast <plist::Dictionary> ("properties");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (P != nullptr) {
        std::unordered_set<std::string> seen;
        auto unpack = plist::Keys::Unpack("Properties", P, &seen);

        auto GS = unpack.cast <plist::String> ("grid-size");

        if (!unpack.complete(true)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }

        if (GS != nullptr) {
            _gridSize = GridSizes::Parse(GS->value());
        }
    }

    return true;
}

ext::optional<GridSize> GridSizes::
Parse(std::string const &value)
{
    if (value == "small") {
        return GridSize::Small;
    } else if (value == "regular") {
        return GridSize::Regular;
    } else if (value == "large") {
        return GridSize::Large;
    } else {
        fprintf(stderr, "warning: unknown platform %s\n", value.c_str());
        return ext::nullopt;
    }
}

std::string GridSizes::
String(GridSize platform)
{
    switch (platform) {
        case GridSize::Small:
            return "small";
        case GridSize::Regular:
            return "regular";
        case GridSize::Large:
            return "large";
    }

    abort();
}
