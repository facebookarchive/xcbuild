/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/CompileAction.h>
#include <acdriver/Compile/Output.h>
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
#include <acdriver/Version.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>
#include <xcassets/Asset/Catalog.h>
#include <xcassets/Asset/Group.h>
#include <bom/bom.h>
#include <car/Reader.h>
#include <car/Writer.h>
#include <bom/bom_format.h>
#include <dependency/BinaryDependencyInfo.h>
#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <plist/Format/XML.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

using acdriver::CompileAction;
namespace Compile = acdriver::Compile;
using acdriver::Version;
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
CompileAsset(
    std::shared_ptr<xcassets::Asset::Asset> const &asset,
    std::shared_ptr<xcassets::Asset::Asset> const &parent,
    Filesystem *filesystem,
    Options const &options,
    Compile::Output *compileOutput,
    Result *result);

template<typename T>
static bool
CompileChildren(
    std::vector<T> const &assets,
    std::shared_ptr<xcassets::Asset::Asset> const &parent,
    Filesystem *filesystem,
    Options const &options,
    Compile::Output *compileOutput,
    Result *result)
{
    bool success = true;

    for (auto const &asset : assets) {
        if (!CompileAsset(asset, parent, filesystem, options, compileOutput, result)) {
            success = false;
        }
    }

    return success;
}

static bool
CompileAsset(
    std::shared_ptr<xcassets::Asset::Asset> const &asset,
    std::shared_ptr<xcassets::Asset::Asset> const &parent,
    Filesystem *filesystem,
    Options const &options,
    Compile::Output *compileOutput,
    Result *result)
{
    std::string filename = FSUtil::GetBaseName(asset->path());
    std::string name = FSUtil::GetBaseNameWithoutExtension(filename);
    switch (asset->type()) {
        case xcassets::Asset::AssetType::AppIconSet: {
            auto appIconSet = std::static_pointer_cast<xcassets::Asset::AppIconSet>(asset);
            if (appIconSet->name().name() == options.appIcon()) {
                Compile::AppIconSet::Compile(appIconSet, filesystem, compileOutput, result);
            }
            break;
        }
        case xcassets::Asset::AssetType::BrandAssets: {
            auto brandAssets = std::static_pointer_cast<xcassets::Asset::BrandAssets>(asset);
            Compile::BrandAssets::Compile(brandAssets, filesystem, compileOutput, result);
            CompileChildren(brandAssets->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::Catalog: {
            auto catalog = std::static_pointer_cast<xcassets::Asset::Catalog>(asset);
            CompileChildren(catalog->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::ComplicationSet: {
            auto complicationSet = std::static_pointer_cast<xcassets::Asset::ComplicationSet>(asset);
            Compile::ComplicationSet::Compile(complicationSet, filesystem, compileOutput, result);
            CompileChildren(complicationSet->children(), asset, filesystem, options, compileOutput, result);
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
            CompileChildren(dashboardImage->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboard: {
            auto leaderboard = std::static_pointer_cast<xcassets::Asset::GCLeaderboard>(asset);
            Compile::GCLeaderboard::Compile(leaderboard, filesystem, compileOutput, result);
            CompileChildren(leaderboard->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::GCLeaderboardSet: {
            auto leaderboardSet = std::static_pointer_cast<xcassets::Asset::GCLeaderboardSet>(asset);
            Compile::GCLeaderboardSet::Compile(leaderboardSet, filesystem, compileOutput, result);
            CompileChildren(leaderboardSet->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::Group: {
            auto group = std::static_pointer_cast<xcassets::Asset::Group>(asset);
            CompileChildren(group->children(), asset, filesystem, options, compileOutput, result);
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
            CompileChildren(imageStack->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::ImageStackLayer: {
            auto imageStackLayer = std::static_pointer_cast<xcassets::Asset::ImageStackLayer>(asset);
            Compile::ImageStackLayer::Compile(imageStackLayer, filesystem, compileOutput, result);
            // TODO: CompileChildren(imageStackLayer->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
        case xcassets::Asset::AssetType::LaunchImage: {
            auto launchImage = std::static_pointer_cast<xcassets::Asset::LaunchImage>(asset);
            if (launchImage->name().name() == options.launchImage()) {
                Compile::LaunchImage::Compile(launchImage, filesystem, compileOutput, result);
            }
            break;
        }
        case xcassets::Asset::AssetType::SpriteAtlas: {
            auto spriteAtlas = std::static_pointer_cast<xcassets::Asset::SpriteAtlas>(asset);
            Compile::SpriteAtlas::Compile(spriteAtlas, filesystem, compileOutput, result);
            CompileChildren(spriteAtlas->children(), asset, filesystem, options, compileOutput, result);
            break;
        }
    }

    return true;
}

static bool
WriteOutput(Filesystem *filesystem, Options const &options, Compile::Output const &compileOutput, Output *output, Result *result)
{
    bool success = true;

    /*
     * Collect all inputs and outputs.
     */
    auto info = dependency::DependencyInfo(compileOutput.inputs(), compileOutput.outputs());

    /*
     * Write out compiled archive.
     */
    if (compileOutput.car()) {
        // TODO: only write if non-empty. but did mmap already create the file?
        compileOutput.car()->write();
    }

    /*
     * Copy files into output.
     */
    for (std::pair<std::string, std::string> const &copy : compileOutput.copies()) {
        std::vector<uint8_t> contents;

        if (!filesystem->read(&contents, copy.first)) {
            result->normal(Result::Severity::Error, "unable to read input: " + copy.first);
            success = false;
            continue;
        }

        if (!filesystem->write(contents, copy.second)) {
            result->normal(Result::Severity::Error, "unable to write output: " + copy.second);
            success = false;
            continue;
        }
    }

    /*
     * Write out partial info plist, if requested.
     */
    if (options.outputPartialInfoPlist()) {
        auto format = plist::Format::XML::Create(plist::Format::Encoding::UTF8);
        auto serialize = plist::Format::XML::Serialize(compileOutput.additionalInfo(), format);
        if (serialize.first == nullptr) {
            result->normal(Result::Severity::Error, "unable to serialize partial info plist");
            success = false;
        } else {
            if (!filesystem->write(*serialize.first, *options.outputPartialInfoPlist())) {
                result->normal(Result::Severity::Error, "unable to write partial info plist");
                success = false;
            }
        }

        /* Note output file. */
        info.outputs().push_back(*options.outputPartialInfoPlist());
    }

    /*
     * Write out dependency info, if requested.
     */
    if (options.exportDependencyInfo()) {
        auto binaryInfo = dependency::BinaryDependencyInfo();
        binaryInfo.version() = "actool-" + std::to_string(Version::BuildVersion());
        binaryInfo.dependencyInfo() = info;

        if (!filesystem->write(binaryInfo.serialize(), *options.exportDependencyInfo())) {
            result->normal(Result::Severity::Error, "unable to write dependency info");
            success = false;
        }
    }

    /*
     * Add output files to output.
     */
    {
        std::string text;
        auto array = plist::Array::New();

        for (std::string const &output : info.outputs()) {
            /* Array is one entry per file. */
            array->append(plist::String::New(output));

            /* Text is one line per file. */
            text += output;
            text += "\n";
        }

        auto dict = plist::Dictionary::New();
        dict->set("output-files", std::move(array));

        output->add("com.apple.actool.compilation-results", std::move(dict), text);
    }

    return success;
}

static ext::optional<Compile::Output::Format>
DetermineOutputFormat(ext::optional<std::string> const &minimumDeploymentTarget)
{
    if (minimumDeploymentTarget) {
        if (auto systemVersion = xcassets::Slot::SystemVersion::Parse(*minimumDeploymentTarget)) {
            /*
             * Only versions starting with 7 support compiled assets.
             */
            if (systemVersion->major() >= 7) {
                return Compile::Output::Format::Compiled;
            } else {
                return Compile::Output::Format::Folder;
            }
        } else {
            return ext::nullopt;
        }
    } else {
        /* If no version is specified, default to compiled. */
        return Compile::Output::Format::Compiled;
    }
}

static ext::optional<car::Writer>
CreateWriter(std::string const &path)
{
    struct bom_context_memory memory = bom_context_memory_file(path.c_str(), true, 0);
    if (memory.data == NULL) {
        return ext::nullopt;
    }

    auto bom = car::Writer::unique_ptr_bom(bom_alloc_empty(memory), bom_free);
    if (bom == nullptr) {
        return ext::nullopt;
    }

    return car::Writer::Create(std::move(bom));
}

static void
WarnUnsupportedOptions(Options const &options, Result *result)
{
    if (options.optimization()) {
        result->normal(Result::Severity::Warning, "optimization not supported");
    }

    if (options.compressPNGs()) {
        result->normal(Result::Severity::Warning, "compress PNGs not supported");
    }

    if (options.platform()) {
        result->normal(Result::Severity::Warning, "platform not supported");
    }

    if (!options.targetDevice().empty()) {
        result->normal(Result::Severity::Warning, "target device not supported");
    }

    if (options.targetName()) {
        result->normal(Result::Severity::Warning, "target name not supported");
    }

    if (options.enableOnDemandResources()) {
        result->normal(Result::Severity::Warning, "on-demand resources not supported");
    }

    if (options.enableIncrementalDistill()) {
        result->normal(Result::Severity::Warning, "incremental distill not supported");
    }

    if (options.filterForDeviceModel()) {
        result->normal(Result::Severity::Warning, "filter device model not supported");
    }

    if (options.filterForDeviceOsVersion()) {
        result->normal(Result::Severity::Warning, "filter device os version not supported");
    }
}

void CompileAction::
run(Filesystem *filesystem, Options const &options, Output *output, Result *result)
{
    // TODO: support all options
    WarnUnsupportedOptions(options, result);

    /*
     * Determine format to output compiled assets.
     */
    ext::optional<Compile::Output::Format> outputFormat = DetermineOutputFormat(options.minimumDeploymentTarget());
    if (!outputFormat) {
        result->normal(Result::Severity::Error, "invalid minimum deployment target");
        return;
    }

    /*
     * Create compilation output.
     */
    Compile::Output compileOutput = Compile::Output(*options.compile(), *outputFormat);

    /*
     * If necessary, create output archive to write into.
     */
    if (compileOutput.format() == Compile::Output::Format::Compiled) {
        std::string path = compileOutput.root() + "/" + "Assets.car";

        ext::optional<car::Writer> writer = CreateWriter(path);
        if (!writer) {
            result->normal(Result::Severity::Error, "unable to create compiled asset writer");
            return;
        }

        compileOutput.car() = std::move(writer);
        // TODO: should only be an output if ultimately non-empty
        compileOutput.outputs().push_back(path);
    }

    /*
     * Compile each asset catalog into the output.
     */
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
         * Compile the asset catalog.
         */
        if (!CompileAsset(catalog, catalog, filesystem, options, &compileOutput, result)) {
            /* Error already printed. */
            continue;
        }

        compileOutput.inputs().push_back(input);
    }

    /*
     * Write out the output.
     */
    if (!WriteOutput(filesystem, options, compileOutput, output, result)) {
        /* Error already reported. */
        return;
    }
}

