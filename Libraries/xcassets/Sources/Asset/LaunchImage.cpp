/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset/LaunchImage.h>
#include <plist/Keys/Unpack.h>
#include <plist/Array.h>
#include <plist/String.h>

using xcassets::Asset::LaunchImage;

bool LaunchImage::Image::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("LaunchImageImage", dict, &seen);

    auto F  = unpack.cast <plist::String> ("filename");
    auto I  = unpack.cast <plist::String> ("idiom");
    auto O  = unpack.cast <plist::String> ("orientation");
    auto S  = unpack.cast <plist::String> ("scale");
    auto ST = unpack.cast <plist::String> ("subtype");
    // TODO: minimum-system-version
    auto E  = unpack.cast <plist::String> ("extent");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (F != nullptr) {
        _fileName = F->value();
    }

    if (I != nullptr) {
        _idiom = Slot::Idioms::Parse(I->value());
    }

    if (O != nullptr) {
        _orientation = Slot::Orientations::Parse(O->value());
    }

    if (S != nullptr) {
        _scale = Slot::Scale::Parse(S->value());
    }

    if (ST != nullptr) {
        _subtype = Slot::DeviceSubtypes::Parse(ST->value());
    }

    if (E != nullptr) {
        _extent = Slot::LaunchImageExtents::Parse(E->value());
    }

    return true;
}

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

    auto Is = unpack.cast <plist::Array> ("images");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (Is != nullptr) {
        _images = std::vector<Image>();

        for (size_t n = 0; n < Is->count(); ++n) {
            if (auto dict = Is->value<plist::Dictionary>(n)) {
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

