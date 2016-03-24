/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/DataSet.h>
#include <plist/Keys/Unpack.h>
#include <plist/Array.h>
#include <plist/String.h>

using xcassets::DataSet;

bool DataSet::Data::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("ImageSetImage", dict, &seen);

    auto F   = unpack.cast <plist::String> ("filename");
    auto I   = unpack.cast <plist::String> ("idiom");
    auto GFS = unpack.cast <plist::String> ("graphics-feature-set");
    // TODO: memory
    auto UTI = unpack.cast <plist::String> ("universal-type-identifier");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (F != nullptr) {
        _fileName = F->value();
    }

    if (I != nullptr) {
        _idiom = Idioms::Parse(I->value());
    }

    if (GFS != nullptr) {
        _graphicsFeatureSet = GraphicsFeatureSets::Parse(GFS->value());
    }

    if (UTI != nullptr) {
        _UTI = UTI->value();
    }

    return true;
}

bool DataSet::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("DataSet", dict, seen);

    auto P  = unpack.cast <plist::Dictionary> ("properties");
    auto Ds = unpack.cast <plist::Array> ("data");

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

    if (Ds != nullptr) {
        _data = std::vector<Data>();

        for (size_t n = 0; n < Ds->count(); ++n) {
            if (auto dict = Ds->value<plist::Dictionary>(n)) {
                Data data;
                if (data.parse(dict)) {
                    _data->push_back(data);
                }
            }
        }
    }

    // TODO: confirm no children

    return true;
}



