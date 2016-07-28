/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/Compile/Asset.h>
#include <acdriver/Compile/AppIconSet.h>
#include <acdriver/Compile/BrandAssets.h>
#include <acdriver/Compile/ComplicationSet.h>
#include <acdriver/Compile/DataSet.h>
#include <acdriver/Compile/GCDashboardImage.h>
#include <acdriver/Compile/GCLeaderboard.h>
#include <acdriver/Compile/GCLeaderboardSet.h>
#include <acdriver/Compile/IconSet.h>
#include <acdriver/Compile/ImageSet.h>
#include <acdriver/Compile/ImageStack.h>
#include <acdriver/Compile/ImageStackLayer.h>
#include <acdriver/Compile/LaunchImage.h>
#include <acdriver/Compile/SpriteAtlas.h>
#include <acdriver/Compile/Output.h>
#include <acdriver/Result.h>
#include <xcassets/Asset/Catalog.h>
#include <xcassets/Asset/Group.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

using acdriver::Compile::Asset;
using acdriver::Compile::AppIconSet;
using acdriver::Compile::BrandAssets;
using acdriver::Compile::ComplicationSet;
using acdriver::Compile::DataSet;
using acdriver::Compile::GCDashboardImage;
using acdriver::Compile::GCLeaderboard;
using acdriver::Compile::GCLeaderboardSet;
using acdriver::Compile::IconSet;
using acdriver::Compile::ImageSet;
using acdriver::Compile::ImageStack;
using acdriver::Compile::ImageStackLayer;
using acdriver::Compile::LaunchImage;
using acdriver::Compile::SpriteAtlas;
using acdriver::Compile::Output;
using acdriver::Result;
using libutil::Filesystem;
using libutil::FSUtil;

template<typename T>
static bool
CompileChildren(
    std::vector<T> const &assets,
    std::shared_ptr<xcassets::Asset::Asset> const &asset,
    Filesystem *filesystem,
    Output *compileOutput,
    Result *result)
{
    bool success = true;

    for (auto const &asset : assets) {
        if (!Asset::Compile(asset, filesystem, compileOutput, result)) {
            success = false;
        }
    }

    return success;
}

bool Asset::
Compile(
    std::shared_ptr<xcassets::Asset::Asset> const &asset,
    Filesystem *filesystem,
    Output *compileOutput,
    Result *result)
{
    std::string filename = FSUtil::GetBaseName(asset->path());
    std::string name = FSUtil::GetBaseNameWithoutExtension(filename);

    switch (asset->type()) {
        case xcassets::Asset::AssetType::AppIconSet: {
            auto appIconSet = std::static_pointer_cast<xcassets::Asset::AppIconSet>(asset);
            if (appIconSet->name().name() == compileOutput->appIcon()) {
                Compile::AppIconSet::Compile(appIconSet, compileOutput, result);
            }
            break;
        }
        case xcassets::Asset::AssetType::BrandAssets: {
            auto brandAssets = std::static_pointer_cast<xcassets::Asset::BrandAssets>(asset);
            Compile::BrandAssets::Compile(brandAssets, filesystem, compileOutput, result);
            CompileChildren(brandAssets->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::Catalog: {
            auto catalog = std::static_pointer_cast<xcassets::Asset::Catalog>(asset);
            CompileChildren(catalog->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::ComplicationSet: {
            auto complicationSet = std::static_pointer_cast<xcassets::Asset::ComplicationSet>(asset);
            Compile::ComplicationSet::Compile(complicationSet, filesystem, compileOutput, result);
            CompileChildren(complicationSet->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::DataSet: {
            auto dataSet = std::static_pointer_cast<xcassets::Asset::DataSet>(asset);
            Compile::DataSet::Compile(dataSet, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::GCDashboardImage: {
            auto dashboardImage = std::static_pointer_cast<xcassets::Asset::GCDashboardImage>(asset);
            Compile::GCDashboardImage::Compile(dashboardImage, filesystem, compileOutput, result);
            CompileChildren(dashboardImage->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboard: {
            auto leaderboard = std::static_pointer_cast<xcassets::Asset::GCLeaderboard>(asset);
            Compile::GCLeaderboard::Compile(leaderboard, filesystem, compileOutput, result);
            CompileChildren(leaderboard->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboardSet: {
            auto leaderboardSet = std::static_pointer_cast<xcassets::Asset::GCLeaderboardSet>(asset);
            Compile::GCLeaderboardSet::Compile(leaderboardSet, filesystem, compileOutput, result);
            CompileChildren(leaderboardSet->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::Group: {
            auto group = std::static_pointer_cast<xcassets::Asset::Group>(asset);
            CompileChildren(group->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::IconSet: {
            auto iconSet = std::static_pointer_cast<xcassets::Asset::IconSet>(asset);
            Compile::IconSet::Compile(iconSet, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::ImageSet: {
            auto imageSet = std::static_pointer_cast<xcassets::Asset::ImageSet>(asset);
            Compile::ImageSet::Compile(imageSet, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::ImageStack: {
            auto imageStack = std::static_pointer_cast<xcassets::Asset::ImageStack>(asset);
            Compile::ImageStack::Compile(imageStack, filesystem, compileOutput, result);
            CompileChildren(imageStack->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::ImageStackLayer: {
            auto imageStackLayer = std::static_pointer_cast<xcassets::Asset::ImageStackLayer>(asset);
            Compile::ImageStackLayer::Compile(imageStackLayer, filesystem, compileOutput, result);
            // TODO: CompileChildren(imageStackLayer->children(), asset, filesystem, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::LaunchImage: {
            auto launchImage = std::static_pointer_cast<xcassets::Asset::LaunchImage>(asset);
            if (launchImage->name().name() == compileOutput->launchImage()) {
                Compile::LaunchImage::Compile(launchImage, compileOutput, result);
            }
            break;
        }
        case xcassets::Asset::AssetType::SpriteAtlas: {
            auto spriteAtlas = std::static_pointer_cast<xcassets::Asset::SpriteAtlas>(asset);
            Compile::SpriteAtlas::Compile(spriteAtlas, filesystem, compileOutput, result);
            CompileChildren(spriteAtlas->children(), asset, filesystem, compileOutput, result);
            break;
        }
    }

    return true;
}

