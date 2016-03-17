/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <xcassets/Asset.h>
#include <xcassets/AppIconSet.h>
#include <xcassets/BrandAssets.h>
#include <xcassets/Catalog.h>
#include <xcassets/ComplicationSet.h>
#include <xcassets/DataSet.h>
#include <xcassets/GCDashboardImage.h>
#include <xcassets/GCLeaderboard.h>
#include <xcassets/GCLeaderboardSet.h>
#include <xcassets/Group.h>
#include <xcassets/IconSet.h>
#include <xcassets/ImageSet.h>
#include <xcassets/ImageStack.h>
#include <xcassets/ImageStackLayer.h>
#include <xcassets/LaunchImage.h>
#include <xcassets/SpriteAtlas.h>
#include <plist/Keys/Unpack.h>
#include <plist/Format/JSON.h>
#include <plist/String.h>
#include <plist/Integer.h>
#include <libutil/FSUtil.h>

#include <fstream>

using xcassets::Asset;
using libutil::FSUtil;

Asset::
Asset()
{
}

Asset::
~Asset()
{
}

bool Asset::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    /* No contents is allowed for some assets. */
    if (dict == nullptr) {
        return true;
    }

    auto unpack = plist::Keys::Unpack("Asset", dict, seen);

    auto I = unpack.cast <plist::Dictionary> ("info");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (I != nullptr) {
        std::unordered_set<std::string> seen;
        auto unpack = plist::Keys::Unpack("Info", I, &seen);

        auto A = unpack.cast <plist::String> ("author");
        auto V = unpack.cast <plist::Integer> ("version");

        if (!unpack.complete(true)) {
            fprintf(stderr, "%s", unpack.errorText().c_str());
        }

        if (A != nullptr) {
            _author = A->value();
        }

        if (V != nullptr) {
            _version = V->value();

            if (_version != 1) {
                fprintf(stderr, "warning: unknown version: %d", *_version);
                return false;
            }
        }
    }

    return true;
}

bool Asset::
loadChildren(std::vector<std::shared_ptr<Asset>> *children)
{
    bool error = false;

    FSUtil::EnumerateDirectory(_path, [&](std::string const &fileName) -> bool {
        std::string path = _path + "/" + fileName;

        if (FSUtil::TestForDirectory(path)) {
            std::shared_ptr<Asset> asset = Asset::Load(path, _group);
            if (asset == nullptr) {
                fprintf(stderr, "error: failed to load asset: %s\n", path.c_str());
                error = true;
                return true;
            }

            children->push_back(asset);
        }

        return true;
    });

    return !error;
}

std::shared_ptr<Asset> Asset::
Load(std::string const &path, ext::optional<std::string> const &group)
{
    std::string resolvedPath = FSUtil::ResolvePath(path);

    /*
     * Assets are always in directories.
     */
    if (!FSUtil::TestForDirectory(resolvedPath)) {
        return nullptr;
    }

    /*
     * Get the file extension.
     */
    ext::optional<std::string> extension = FSUtil::GetFileExtension(resolvedPath);
    if (extension->empty()) {
        // TODO(grp): Remove when FSUtil returns an optional.
        extension = ext::nullopt;
    }

    /*
     * Create an asset of the right type.
     */
    std::shared_ptr<Asset> asset = nullptr;
    if (extension == AppIconSet::Extension()) {
        auto appIconSet = std::make_shared<AppIconSet>();
        asset = std::static_pointer_cast<Asset>(appIconSet);
    } else if (extension == BrandAssets::Extension()) {
        auto brandAssets = std::make_shared<BrandAssets>();
        asset = std::static_pointer_cast<Asset>(brandAssets);
    } else if (extension == Catalog::Extension()) {
        auto catalog = std::make_shared<Catalog>();
        asset = std::static_pointer_cast<Asset>(catalog);
    } else if (extension == ComplicationSet::Extension()) {
        auto complicationSet = std::make_shared<ComplicationSet>();
        asset = std::static_pointer_cast<Asset>(complicationSet);
    } else if (extension == DataSet::Extension()) {
        auto dataSet = std::make_shared<DataSet>();
        asset = std::static_pointer_cast<Asset>(dataSet);
    } else if (extension == GCDashboardImage::Extension()) {
        auto gcDashboardImage = std::make_shared<GCDashboardImage>();
        asset = std::static_pointer_cast<Asset>(gcDashboardImage);
    } else if (extension == GCLeaderboard::Extension()) {
        auto gcLeaderboard = std::make_shared<GCLeaderboard>();
        asset = std::static_pointer_cast<Asset>(gcLeaderboard);
    } else if (extension == GCLeaderboardSet::Extension()) {
        auto gcLeaderboardSet = std::make_shared<GCLeaderboardSet>();
        asset = std::static_pointer_cast<Asset>(gcLeaderboardSet);
    } else if (extension == Group::Extension()) {
        auto group = std::make_shared<Group>();
        asset = std::static_pointer_cast<Asset>(group);
    } else if (extension == IconSet::Extension()) {
        auto iconSet = std::make_shared<IconSet>();
        asset = std::static_pointer_cast<Asset>(iconSet);
    } else if (extension == ImageSet::Extension()) {
        auto imageSet = std::make_shared<ImageSet>();
        asset = std::static_pointer_cast<Asset>(imageSet);
    } else if (extension == ImageStack::Extension()) {
        auto imageStack = std::make_shared<ImageStack>();
        asset = std::static_pointer_cast<Asset>(imageStack);
    } else if (extension == ImageStackLayer::Extension()) {
        auto imageStackLayer = std::make_shared<ImageStackLayer>();
        asset = std::static_pointer_cast<Asset>(imageStackLayer);
    } else if (extension == LaunchImage::Extension()) {
        auto launchImage = std::make_shared<LaunchImage>();
        asset = std::static_pointer_cast<Asset>(launchImage);
    } else if (extension == SpriteAtlas::Extension()) {
        auto spriteAtlas = std::make_shared<SpriteAtlas>();
        asset = std::static_pointer_cast<Asset>(spriteAtlas);
    } else {
        return nullptr;
    }

    /*
     * Configure the asset with the contents.
     */
    asset->_path = resolvedPath;
    asset->_name = FSUtil::GetBaseNameWithoutExtension(resolvedPath);
    asset->_group = group;

    std::unique_ptr<plist::Dictionary> contentsDictionary;
    std::string contentsPath = path + "/" + "Contents.json";

    /*
     * Check if the contents file exists. Not existing is valid for some asset types.
     */
    std::ifstream contentsFile;
    contentsFile.open(contentsPath, std::ios::binary);
    if (!contentsFile.fail()) {
        std::vector<uint8_t> contents = std::vector<uint8_t>(std::istreambuf_iterator<char>(contentsFile), std::istreambuf_iterator<char>());

        /*
         * If the Contents.json file exists, it must be JSON.
         */
        auto deserialized = plist::Format::JSON::Deserialize(contents, plist::Format::JSON::Create());
        if (!deserialized.first) {
            return nullptr;
        }

        /*
         * If the Contents.json file exists, it must be a dictionary.
         */
        if (deserialized.first->type() != plist::Dictionary::Type()) {
            return nullptr;
        }

        contentsDictionary = plist::static_unique_pointer_cast<plist::Dictionary>(std::move(deserialized.first));
    }

    /*
     * Parse the contents dictionary.
     */
    std::unordered_set<std::string> seen;
    if (!asset->parse(contentsDictionary.get(), &seen, true)) {
        return nullptr;
    }

    return asset;
}
