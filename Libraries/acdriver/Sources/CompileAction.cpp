/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/CompileAction.h>
#include <acdriver/CompileActionImageSet.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>
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
#include <xcassets/Slot/Idiom.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <bom/bom.h>
#include <car/Reader.h>
#include <car/Writer.h>
#include <bom/bom_format.h>


using acdriver::CompileAction;
using acdriver::Options;
using acdriver::Output;
using acdriver::Result;

using libutil::Filesystem;
using libutil::FSUtil;


CompileAction::
CompileAction()
{
}

CompileAction::
~CompileAction()
{
}


static bool
CompileContents(ext::optional<car::Writer> &writer, std::string ns, std::shared_ptr<xcassets::Asset::Asset> const &parent, std::shared_ptr<xcassets::Asset::Asset> const &asset, Result *result);

template<typename T>
static void
CompileChildren(ext::optional<car::Writer> &writer, std::string ns, std::shared_ptr<xcassets::Asset::Asset> const &parent, std::vector<T> const &assets, Result *result)
{
    if (assets.empty()) {
        return;
    }

    for (auto const &asset : assets) {
        if (CompileContents(writer, ns, parent, asset, result)) {

        }
    }
}



static bool
CompileContents(ext::optional<car::Writer> &writer, std::string ns, std::shared_ptr<xcassets::Asset::Asset> const &parent, std::shared_ptr<xcassets::Asset::Asset> const &asset, Result *result)
{
    std::string filename = FSUtil::GetBaseName(asset->path());
    std::string name = FSUtil::GetBaseNameWithoutExtension(filename);

    switch (asset->type()) {
        case xcassets::Asset::AssetType::AppIconSet: {
            auto appIconSet = std::static_pointer_cast<xcassets::Asset::AppIconSet>(asset);
            // TODO: App icon set details.
            break;
        }
        case xcassets::Asset::AssetType::BrandAssets: {
            auto brandAssets = std::static_pointer_cast<xcassets::Asset::BrandAssets>(asset);
            CompileChildren(writer, ns, asset, brandAssets->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::Catalog: {
            auto catalog = std::static_pointer_cast<xcassets::Asset::Catalog>(asset);
            CompileChildren(writer, ns, asset, catalog->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::ComplicationSet: {
            auto complicationSet = std::static_pointer_cast<xcassets::Asset::ComplicationSet>(asset);
            CompileChildren(writer, ns, asset, complicationSet->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::DataSet: {
            auto dataSet = std::static_pointer_cast<xcassets::Asset::DataSet>(asset);
            // TODO: Data set details.
            break;
        }
        case xcassets::Asset::AssetType::GCDashboardImage: {
            auto dashboardImage = std::static_pointer_cast<xcassets::Asset::GCDashboardImage>(asset);
            CompileChildren(writer, ns, asset, dashboardImage->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboard: {
            auto leaderboard = std::static_pointer_cast<xcassets::Asset::GCLeaderboard>(asset);
            CompileChildren(writer, ns, asset, leaderboard->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboardSet: {
            auto leaderboardSet = std::static_pointer_cast<xcassets::Asset::GCLeaderboardSet>(asset);
            CompileChildren(writer, ns, asset, leaderboardSet->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::Group: {
            auto group = std::static_pointer_cast<xcassets::Asset::Group>(asset);
            if (group->providesNamespace()) {
                ns = ns + std::string("/") + name;
            }
            CompileChildren(writer, ns, asset, group->children(), result);
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
                CompileChildren(writer, ns, asset, *imageSet->images(), result);
            }
            break;
        }
        case xcassets::Asset::AssetType::ImageStack: {
            auto imageStack = std::static_pointer_cast<xcassets::Asset::ImageStack>(asset);
            CompileChildren(writer, ns, asset, imageStack->children(), result);
            break;
        }
        case xcassets::Asset::AssetType::ImageStackLayer: {
            auto imageStackLayer = std::static_pointer_cast<xcassets::Asset::ImageStackLayer>(asset);
            // TODO: CompileChildren
            break;
        }
        case xcassets::Asset::AssetType::LaunchImage: {
            auto launchImage = std::static_pointer_cast<xcassets::Asset::LaunchImage>(asset);
            // TODO: Launch image details.
            break;
        }
        case xcassets::Asset::AssetType::SpriteAtlas: {
            auto spriteAtlas = std::static_pointer_cast<xcassets::Asset::SpriteAtlas>(asset);
            if (spriteAtlas->providesNamespace()) {
                ns = ns + std::string("/") + name;
            }
            CompileChildren(writer, ns, asset, spriteAtlas->children(), result);
            break;
        }
    }

    return true;
}

void CompileAction::
run(Filesystem const *filesystem, Options const &options, Output *output, Result *result)
{
    std::string compile_output = options.compile() + std::string("/Assets.car");

    // CAR file Writer
    struct bom_context_memory memory_writer = bom_context_memory_file(compile_output.c_str(), true, sizeof(struct bom_header));
    auto bom_writer = std::unique_ptr<struct bom_context, decltype(&bom_free)>(bom_alloc_empty(memory_writer), bom_free);
    if (bom_writer == nullptr) {
        result->normal(
            Result::Severity::Error,
            "unable to load BOM for writing"
            );
        return;
    }
    ext::optional<car::Writer> writer = car::Writer::Create(std::move(bom_writer));

    if (!writer) {
        result->normal(
            Result::Severity::Error,
            "unable to load CAR for writing"
            );
        return;
    }

    for (std::string const &input : options.inputs()) {
        /*
         * Load the input asset catalog.
         */
        auto catalog = xcassets::Asset::Catalog::Load(filesystem, input);
        if (catalog == nullptr) {
            result->normal(
                Result::Severity::Error,
                "unable to load asset catalog during compile",
                ext::nullopt,
                input);
            continue;
        }

        /*
         * Compile, add contents to writer
         */
        if (!CompileContents(writer, std::string(""), catalog, catalog, result)) {
            result->normal(
                Result::Severity::Error,
                "failed to compile asset catalog",
                ext::nullopt,
                input);
        }
    }
    writer->write();
}




