/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcassets/Asset/Asset.h>
#include <xcassets/Asset/AppIconSet.h>
#include <xcassets/Asset/BrandAssets.h>
#include <xcassets/Asset/Catalog.h>
#include <xcassets/Asset/ComplicationSet.h>
#include <xcassets/Asset/DataSet.h>
#include <xcassets/Asset/GCDashboardImage.h>
#include <xcassets/Asset/GCLeaderboard.h>
#include <xcassets/Asset/GCLeaderboardSet.h>
#include <xcassets/Asset/Group.h>
#include <xcassets/Asset/IconSet.h>
#include <xcassets/Asset/ImageSet.h>
#include <xcassets/Asset/ImageStack.h>
#include <xcassets/Asset/ImageStackLayer.h>
#include <xcassets/Asset/LaunchImage.h>
#include <xcassets/Asset/SpriteAtlas.h>
#include <plist/Keys/Unpack.h>
#include <plist/Format/JSON.h>
#include <plist/String.h>
#include <plist/Integer.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

using xcassets::Asset::Asset;
using libutil::Filesystem;
using libutil::FSUtil;

Asset::
Asset(FullyQualifiedName const &name, std::string const &path) :
    _name(name),
    _path(path)
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
loadChildren(Filesystem const *filesystem, std::vector<std::shared_ptr<Asset>> *children, bool providesNamespace)
{
    bool error = false;

    filesystem->enumerateDirectory(_path, [&](std::string const &fileName) -> bool {
        std::string path = _path + "/" + fileName;

        if (filesystem->isDirectory(path)) {
            std::vector<std::string> groups = _name.groups();
            if (providesNamespace) {
                // TODO: Should fully qualified names include extensions?
                groups.push_back(_name.name());
            }

            std::shared_ptr<Asset> asset = Asset::Load(filesystem, path, groups);
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

bool Asset::
hasChildren(libutil::Filesystem const *filesystem)
{
    bool hasChildren = false;

    filesystem->enumerateDirectory(_path, [this, filesystem, &hasChildren](std::string const &fileName) -> bool {
        std::string path = _path + "/" + fileName;
        if (filesystem->isDirectory(path)) {
            hasChildren = true;
        }

        return true;
    });

    return hasChildren;
}

std::shared_ptr<Asset> Asset::
Load(Filesystem const *filesystem, std::string const &path, std::vector<std::string> const &groups)
{
    std::string resolvedPath = filesystem->resolvePath(path);
    FullyQualifiedName name = FullyQualifiedName(groups, FSUtil::GetBaseNameWithoutExtension(path));

    /*
     * Assets are always in directories.
     */
    if (!filesystem->isDirectory(resolvedPath)) {
        return nullptr;
    }

    /*
     * Get the file extension.
     */
    ext::optional<std::string> extension = FSUtil::GetFileExtension(resolvedPath);
    if (extension->empty()) {
        extension = ext::nullopt;
    }

    /*
     * Create an asset of the right type.
     */
    std::shared_ptr<Asset> asset = nullptr;
    if (extension == AppIconSet::Extension()) {
        auto appIconSet = std::shared_ptr<AppIconSet>(new AppIconSet(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(appIconSet);
    } else if (extension == BrandAssets::Extension()) {
        auto brandAssets = std::shared_ptr<BrandAssets>(new BrandAssets(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(brandAssets);
    } else if (extension == Catalog::Extension()) {
        auto catalog = std::shared_ptr<Catalog>(new Catalog(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(catalog);
    } else if (extension == ComplicationSet::Extension()) {
        auto complicationSet = std::shared_ptr<ComplicationSet>(new ComplicationSet(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(complicationSet);
    } else if (extension == DataSet::Extension()) {
        auto dataSet = std::shared_ptr<DataSet>(new DataSet(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(dataSet);
    } else if (extension == GCDashboardImage::Extension()) {
        auto gcDashboardImage = std::shared_ptr<GCDashboardImage>(new GCDashboardImage(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(gcDashboardImage);
    } else if (extension == GCLeaderboard::Extension()) {
        auto gcLeaderboard = std::shared_ptr<GCLeaderboard>(new GCLeaderboard(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(gcLeaderboard);
    } else if (extension == GCLeaderboardSet::Extension()) {
        auto gcLeaderboardSet = std::shared_ptr<GCLeaderboardSet>(new GCLeaderboardSet(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(gcLeaderboardSet);
    } else if (extension == Group::Extension()) {
        auto group = std::shared_ptr<Group>(new Group(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(group);
    } else if (extension == IconSet::Extension()) {
        auto iconSet = std::shared_ptr<IconSet>(new IconSet(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(iconSet);
    } else if (extension == ImageSet::Extension()) {
        auto imageSet = std::shared_ptr<ImageSet>(new ImageSet(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(imageSet);
    } else if (extension == ImageStack::Extension()) {
        auto imageStack = std::shared_ptr<ImageStack>(new ImageStack(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(imageStack);
    } else if (extension == ImageStackLayer::Extension()) {
        auto imageStackLayer = std::shared_ptr<ImageStackLayer>(new ImageStackLayer(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(imageStackLayer);
    } else if (extension == LaunchImage::Extension()) {
        auto launchImage = std::shared_ptr<LaunchImage>(new LaunchImage(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(launchImage);
    } else if (extension == SpriteAtlas::Extension()) {
        auto spriteAtlas = std::shared_ptr<SpriteAtlas>(new SpriteAtlas(name, resolvedPath));
        asset = std::static_pointer_cast<Asset>(spriteAtlas);
    } else {
        return nullptr;
    }

    if (!asset->load(filesystem)) {
        return nullptr;
    }

    return asset;
}

bool Asset::
load(Filesystem const *filesystem)
{
    /*
     * Configure the asset with the contents.
     */
    std::unique_ptr<plist::Dictionary> contentsDictionary;
    std::string contentsPath = _path + "/" + "Contents.json";

    /*
     * Check if the contents file exists. Not existing is valid for some asset types.
     */
    if (filesystem->isReadable(contentsPath)) {
        /*
         * Read in the contents file.
         */
        std::vector<uint8_t> contents;
        if (!filesystem->read(&contents, contentsPath)) {
            return false;
        }

        /*
         * If the Contents.json file exists, it must be JSON.
         */
        auto deserialized = plist::Format::JSON::Deserialize(contents, plist::Format::JSON::Create());
        if (!deserialized.first) {
            return false;
        }

        /*
         * If the Contents.json file exists, it must be a dictionary.
         */
        if (deserialized.first->type() != plist::Dictionary::Type()) {
            return false;
        }

        contentsDictionary = plist::static_unique_pointer_cast<plist::Dictionary>(std::move(deserialized.first));
    }

    /*
     * Parse the contents dictionary.
     */
    std::unordered_set<std::string> seen;
    if (!this->parse(contentsDictionary.get(), &seen, true)) {
        return false;
    }

    return true;
}
