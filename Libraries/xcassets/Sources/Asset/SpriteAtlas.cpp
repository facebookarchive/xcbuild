/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset/SpriteAtlas.h>
#include <xcassets/Asset/ImageSet.h>
#include <plist/Keys/Unpack.h>
#include <plist/Array.h>
#include <plist/String.h>
#include <plist/Boolean.h>

using xcassets::Asset::SpriteAtlas;
using xcassets::Asset::ImageSet;

bool SpriteAtlas::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* No contents is allowed for groups. */
    if (dict != nullptr) {
        auto unpack = plist::Keys::Unpack("SpriteAtlas", dict, seen);

        auto P = unpack.cast <plist::Dictionary> ("properties");

        if (!unpack.complete(check)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }

        if (P != nullptr) {
            std::unordered_set<std::string> seen;
            auto unpack = plist::Keys::Unpack("Properties", P, &seen);

            auto ODRT = unpack.cast <plist::Array> ("on-demand-resource-tags");
            auto PN   = unpack.cast <plist::Boolean> ("provides-namespace");

            if (!unpack.complete(true)) {
                fprintf(stderr, "%s", unpack.errorText().c_str());
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

            if (PN != nullptr) {
                _providesNamespace = PN->value();
            }
        }
    }

    if (!loadChildren<ImageSet>(&_children)) {
        fprintf(stderr, "error: failed to load children\n");
    }

    return true;
}

