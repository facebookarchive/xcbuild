/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/ImageSet.h>
#include <plist/Keys/Unpack.h>
#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/String.h>

using xcassets::ImageSet;

bool ImageSet::Image::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("ImageSetImage", dict, &seen);

    auto F  = unpack.cast <plist::String> ("filename");
    // TODO: slot components
    auto U  = unpack.cast <plist::Boolean> ("unassigned");
    auto AI = unpack.cast <plist::Dictionary> ("alignment-insets");
    auto R  = unpack.cast <plist::Dictionary> ("resizing");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (F != nullptr) {
        _fileName = F->value();
    }

    if (U != nullptr) {
        _unassigned = U->value();
    }

    if (AI != nullptr) {
        Insets alignmentInsets;
        if (alignmentInsets.parse(AI)) {
            _alignmentInsets = alignmentInsets;
        }
    }

    if (R != nullptr) {
        Resizing resizing;
        if (resizing.parse(R)) {
            _resizing = resizing;
        }
    }

    return true;
}

bool ImageSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("ImageSet", dict, seen);

    auto P = unpack.cast <plist::Dictionary> ("properties");
    auto D = unpack.cast <plist::Array> ("images");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (P != nullptr) {
        std::unordered_set<std::string> seen;
        auto unpack = plist::Keys::Unpack("Properties", P, &seen);

        auto ODRT = unpack.cast <plist::Array> ("on-demand-resource-tags");

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
    }

    if (D != nullptr) {
        _images = std::vector<Image>();

        for (size_t n = 0; n < D->count(); ++n) {
            if (auto dict = D->value<plist::Dictionary>(n)) {
                Image image;
                if (image.parse(dict)) {
                    _images->push_back(image);
                }
            }
        }
    }

    // TODO: confirm no children

    return true;
}


