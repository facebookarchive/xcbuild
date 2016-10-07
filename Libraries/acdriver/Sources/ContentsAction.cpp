/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/ContentsAction.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>
#include <xcassets/Asset/AppIconSet.h>
#include <xcassets/Asset/BrandAssets.h>
#include <xcassets/Asset/Catalog.h>
#include <xcassets/Asset/ComplicationSet.h>
#include <xcassets/Asset/CubeTextureSet.h>
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
#include <xcassets/Asset/MipmapSet.h>
#include <xcassets/Asset/SpriteAtlas.h>
#include <xcassets/Asset/Sticker.h>
#include <xcassets/Asset/StickerPack.h>
#include <xcassets/Asset/StickerSequence.h>
#include <xcassets/Asset/Stickers.h>
#include <xcassets/Asset/StickersIconSet.h>
#include <xcassets/Asset/TextureSet.h>
#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/Dictionary.h>
#include <plist/Object.h>
#include <plist/String.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

using acdriver::ContentsAction;
using acdriver::Options;
using acdriver::Output;
using acdriver::Result;
using libutil::Filesystem;
using libutil::FSUtil;

ContentsAction::
ContentsAction()
{
}

ContentsAction::
~ContentsAction()
{
}

static void
AppendText(std::string *text, int indent, std::string const &add)
{
    for (int i = 0; i < indent; i++) {
        *text += "        ";
    }
    *text += add;
    *text += "\n";
}

static bool
AppendContents(plist::Dictionary *dict, std::string *text, int indent, std::shared_ptr<xcassets::Asset::Asset> const &asset);

static bool
AppendContents(plist::Dictionary *dict, std::string *text, int indent, xcassets::Asset::ImageSet::Image const &image);

template<typename T>
static void
AppendChildren(plist::Dictionary *dict, std::string *text, int indent, std::vector<T> const &assets)
{
    if (assets.empty()) {
        return;
    }

    auto children = plist::Array::New();
    AppendText(text, indent, "children: ");

    bool separator = false;
    for (auto const &asset : assets) {
        auto child = plist::Dictionary::New();
        std::string childText;
        if (AppendContents(child.get(), &childText, indent + 1, asset)) {
            children->append(std::move(child));

            /*
             * Newline before each entry except the first. The effect is to separate each entry in
             * a list of children by a newline, but to avoid adding additional newlines to the end.
             */
            if (!separator) {
                separator = true;
            } else {
                AppendText(text, indent + 1, "");
            }
            *text += childText;
        }
    }

    auto object = plist::static_unique_pointer_cast<plist::Object>(std::move(children));
    dict->set("children", std::move(object));
}

static void
AppendContentReference(plist::Dictionary *dict, std::string *text, int indent, ext::optional<xcassets::ContentReference> const &contentReference)
{
    if (contentReference && contentReference->name()) {
        dict->set("contentReference", plist::String::New(contentReference->name()->name()));
        AppendText(text, indent, "contentReference: " + contentReference->name()->name());
    }
}

static void
AppendProvidesNamespace(plist::Dictionary *dict, std::string *text, int indent, bool providesNamespace)
{
    if (providesNamespace) {
        dict->set("provides-namespace", plist::Boolean::New(true));
        AppendText(text, indent, "provides-namespace: 1");
    }
}

static void
AppendFilename(plist::Dictionary *dict, std::string *text, int indent, std::string const &filename)
{
    dict->set("filename", plist::String::New(filename));
    AppendText(text, indent, "filename: " + filename);
}

static bool
AppendContents(plist::Dictionary *dict, std::string *text, int indent, xcassets::Asset::ImageSet::Image const &image)
{
    /* Skip images that aren't attached to a file. */
    if (!image.fileName()) {
        return false;
    }

    AppendFilename(dict, text, indent, *image.fileName());

    if (image.scale()) {
        std::string scale = xcassets::Slot::Scale::String(*image.scale());
        dict->set("scale", plist::String::New(scale));
        AppendText(text, indent, "scale: " + scale);
    }

    if (image.idiom()) {
        std::string idiom = xcassets::Slot::Idioms::String(*image.idiom());
        dict->set("idiom", plist::String::New(idiom));
        AppendText(text, indent, "idiom: " + idiom);
    }

    // TODO: image: 
    // TODO:        width
    // TODO:        height

    return true;
}

static bool
AppendContents(plist::Dictionary *dict, std::string *text, int indent, std::shared_ptr<xcassets::Asset::Asset> const &asset)
{
    /* Includes the file extension. */
    std::string filename = FSUtil::GetBaseName(asset->path());
    AppendFilename(dict, text, indent, filename);

    switch (asset->type()) {
        case xcassets::Asset::AssetType::AppIconSet: {
            auto appIconSet = std::static_pointer_cast<xcassets::Asset::AppIconSet>(asset);
            break;
        }
        case xcassets::Asset::AssetType::BrandAssets: {
            auto brandAssets = std::static_pointer_cast<xcassets::Asset::BrandAssets>(asset);
            AppendChildren(dict, text, indent, brandAssets->children());
            break;
        }
        case xcassets::Asset::AssetType::Catalog: {
            auto catalog = std::static_pointer_cast<xcassets::Asset::Catalog>(asset);
            AppendChildren(dict, text, indent, catalog->children());
            break;
        }
        case xcassets::Asset::AssetType::ComplicationSet: {
            auto complicationSet = std::static_pointer_cast<xcassets::Asset::ComplicationSet>(asset);
            AppendChildren(dict, text, indent, complicationSet->children());
            break;
        }
        case xcassets::Asset::AssetType::CubeTextureSet: {
            auto cubeTextureSet = std::static_pointer_cast<xcassets::Asset::CubeTextureSet>(asset);
            // TODO: Cube texture set details.
            AppendChildren(dict, text, indent, cubeTextureSet->children());
            break;
        }
        case xcassets::Asset::AssetType::DataSet: {
            auto dataSet = std::static_pointer_cast<xcassets::Asset::DataSet>(asset);
            // TODO: Data set details.
            break;
        }
        case xcassets::Asset::AssetType::GCDashboardImage: {
            auto dashboardImage = std::static_pointer_cast<xcassets::Asset::GCDashboardImage>(asset);
            AppendContentReference(dict, text, indent, dashboardImage->contentReference());
            AppendChildren(dict, text, indent, dashboardImage->children());
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboard: {
            auto leaderboard = std::static_pointer_cast<xcassets::Asset::GCLeaderboard>(asset);
            AppendContentReference(dict, text, indent, leaderboard->contentReference());
            AppendChildren(dict, text, indent, leaderboard->children());
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboardSet: {
            auto leaderboardSet = std::static_pointer_cast<xcassets::Asset::GCLeaderboardSet>(asset);
            AppendContentReference(dict, text, indent, leaderboardSet->contentReference());
            AppendChildren(dict, text, indent, leaderboardSet->children());
            break;
        }
        case xcassets::Asset::AssetType::Group: {
            auto group = std::static_pointer_cast<xcassets::Asset::Group>(asset);
            AppendProvidesNamespace(dict, text, indent, group->providesNamespace());
            AppendChildren(dict, text, indent, group->children());
            break;
        }
        case xcassets::Asset::AssetType::IconSet: {
            auto iconSet = std::static_pointer_cast<xcassets::Asset::IconSet>(asset);
            // TODO: Icon set details.
            break;
        }
        case xcassets::Asset::AssetType::ImageSet: {
            auto imageSet = std::static_pointer_cast<xcassets::Asset::ImageSet>(asset);
            if (imageSet->images()) {
                AppendChildren(dict, text, indent, *imageSet->images());
            }
            break;
        }
        case xcassets::Asset::AssetType::ImageStack: {
            auto imageStack = std::static_pointer_cast<xcassets::Asset::ImageStack>(asset);
            AppendChildren(dict, text, indent, imageStack->children());
            break;
        }
        case xcassets::Asset::AssetType::ImageStackLayer: {
            auto imageStackLayer = std::static_pointer_cast<xcassets::Asset::ImageStackLayer>(asset);
            AppendContentReference(dict, text, indent, imageStackLayer->contentReference());
            // TODO: AppendChildren(dict, text, indent, imageStackLayer->children());
            break;
        }
        case xcassets::Asset::AssetType::LaunchImage: {
            auto launchImage = std::static_pointer_cast<xcassets::Asset::LaunchImage>(asset);
            // TODO: Launch image details.
            break;
        }
        case xcassets::Asset::AssetType::MipmapSet: {
            auto mipmapSet = std::static_pointer_cast<xcassets::Asset::MipmapSet>(asset);
            // TODO: Mipmap set details.
            break;
        }
        case xcassets::Asset::AssetType::SpriteAtlas: {
            auto spriteAtlas = std::static_pointer_cast<xcassets::Asset::SpriteAtlas>(asset);
            AppendProvidesNamespace(dict, text, indent, spriteAtlas->providesNamespace());
            AppendChildren(dict, text, indent, spriteAtlas->children());
            break;
        }
        case xcassets::Asset::AssetType::Sticker: {
            auto sticker = std::static_pointer_cast<xcassets::Asset::Sticker>(asset);
            // TODO: Sticker details.
            break;
        }
        case xcassets::Asset::AssetType::StickerPack: {
            auto stickerPack = std::static_pointer_cast<xcassets::Asset::StickerPack>(asset);
            // TODO: Sticker pack details.
            AppendChildren(dict, text, indent, stickerPack->children());
            break;
        }
        case xcassets::Asset::AssetType::StickerSequence: {
            auto stickerSequence = std::static_pointer_cast<xcassets::Asset::StickerSequence>(asset);
            // TODO: Sticker sequence details.
            break;
        }
        case xcassets::Asset::AssetType::Stickers: {
            auto stickers = std::static_pointer_cast<xcassets::Asset::Stickers>(asset);
            AppendChildren(dict, text, indent, stickers->children());
            break;
        }
        case xcassets::Asset::AssetType::StickersIconSet: {
            auto stickersIconSet = std::static_pointer_cast<xcassets::Asset::StickersIconSet>(asset);
            // TODO: Stickers icon set details.
            break;
        }
        case xcassets::Asset::AssetType::TextureSet: {
            auto textureSet = std::static_pointer_cast<xcassets::Asset::TextureSet>(asset);
            // TODO: Texture set details.
            AppendChildren(dict, text, indent, textureSet->children());
            break;
        }
    }

    return true;
}

void ContentsAction::
run(Filesystem const *filesystem, Options const &options, Output *output, Result *result)
{
    auto array = plist::Array::New();
    std::string text;

    for (std::string const &input : options.inputs()) {
        /*
         * Load the input asset catalog.
         */
        auto catalog = xcassets::Asset::Catalog::Load(filesystem, input);
        if (catalog == nullptr) {
            result->normal(
                Result::Severity::Error,
                "unable to load asset catalog",
                ext::nullopt,
                input);
            continue;
        }

        /*
         * Create the description of the contents.
         */
        auto dict = plist::Dictionary::New();
        if (AppendContents(dict.get(), &text, 0, catalog)) {
            array->append(std::move(dict));
        }
    }

    /*
     * Store contents in the output.
     */
    auto object = plist::static_unique_pointer_cast<plist::Object>(std::move(array));
    output->add("com.apple.actool.catalog-contents", std::move(object), text);
}

