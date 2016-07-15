/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/Compile/ImageSet.h>
#include <acdriver/CompileOutput.h>
#include <acdriver/Result.h>
#include <graphics/PixelFormat.h>
#include <graphics/Format/PNG.h>
#include <xcassets/Asset/ImageSet.h>
#include <xcassets/Slot/Idiom.h>
#include <car/Facet.h>
#include <car/Rendition.h>
#include <car/Writer.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <string>

using acdriver::Compile::ImageSet;
using acdriver::CompileOutput;
using acdriver::Result;
using libutil::Filesystem;
using libutil::FSUtil;

static uint16_t
IdiomAttributeForIdiom(xcassets::Slot::Idiom assetIdiom)
{
    switch (assetIdiom) {
        case xcassets::Slot::Idiom::Universal:
            return car_attribute_identifier_idiom_value_universal;
            break;
        case xcassets::Slot::Idiom::Phone:
            return car_attribute_identifier_idiom_value_phone;
            break;
        case xcassets::Slot::Idiom::Pad:
            return car_attribute_identifier_idiom_value_pad;
            break;
        case xcassets::Slot::Idiom::Desktop:
            // TODO: desktop has no idiom value
            return car_attribute_identifier_idiom_value_universal;
        case xcassets::Slot::Idiom::TV:
            return car_attribute_identifier_idiom_value_tv;
        case xcassets::Slot::Idiom::Watch:
            return car_attribute_identifier_idiom_value_watch;
        case xcassets::Slot::Idiom::Car:
            return car_attribute_identifier_idiom_value_car;
        default:
            return car_attribute_identifier_idiom_value_universal;
    }
}

static enum car_rendition_value_layout
LayoutForResizingAndCenterMode(xcassets::Resizing::Mode resizingMode, xcassets::Resizing::Center::Mode centerMode)
{
    switch (resizingMode) {
        case xcassets::Resizing::Mode::ThreePartHorizontal:
            switch (centerMode) {
                case xcassets::Resizing::Center::Mode::Tile:
                    return car_rendition_value_layout_three_part_horizontal_tile;
                case xcassets::Resizing::Center::Mode::Stretch:
                    return car_rendition_value_layout_three_part_horizontal_scale;
                default:
                    return car_rendition_value_layout_three_part_horizontal_tile;
            }
        case xcassets::Resizing::Mode::ThreePartVertical:
            switch (centerMode) {
                case xcassets::Resizing::Center::Mode::Tile:
                    return car_rendition_value_layout_three_part_vertical_tile;
                case xcassets::Resizing::Center::Mode::Stretch:
                    return car_rendition_value_layout_three_part_vertical_scale;
                default:
                    return car_rendition_value_layout_three_part_vertical_tile;
            }
        case xcassets::Resizing::Mode::NinePart:
            switch (centerMode) {
                case xcassets::Resizing::Center::Mode::Tile:
                    return car_rendition_value_layout_nine_part_tile;
                case xcassets::Resizing::Center::Mode::Stretch:
                    return car_rendition_value_layout_nine_part_scale;
                default:
                    return car_rendition_value_layout_nine_part_tile;
            }
        default: abort();
    }
}

static std::vector<car::Rendition::Slice>
SlicesForResizingModeAndCapInsets(uint32_t width, uint32_t height, xcassets::Resizing::Mode resizingMode, ext::optional<xcassets::Insets> const &capInsets)
{
    double topCapInset = capInsets ? capInsets->top().value_or(0) : 0;
    double leftCapInset = capInsets ? capInsets->left().value_or(0) : 0;
    double bottomCapInset = capInsets ? capInsets->bottom().value_or(0) : 0;
    double rightCapInset = capInsets ? capInsets->right().value_or(0) : 0;

    uint32_t leftWidth = static_cast<uint32_t>(leftCapInset);
    uint32_t centerWidth = static_cast<uint32_t>(width - (leftCapInset + rightCapInset));
    uint32_t rightWidth = static_cast<uint32_t>(rightCapInset);

    uint32_t topHeight = static_cast<uint32_t>(topCapInset);
    uint32_t centerHeight = static_cast<uint32_t>(height - (topCapInset + bottomCapInset));
    uint32_t bottomHeight = static_cast<uint32_t>(bottomCapInset);

    uint32_t topVerticalOffset = static_cast<uint32_t>(height - topCapInset);
    uint32_t centerVerticalOffset = static_cast<uint32_t>(bottomCapInset);
    uint32_t bottomVerticalOffset = static_cast<uint32_t>(0);

    uint32_t leftHorizontalOffset = static_cast<uint32_t>(0);
    uint32_t centerHorizontalOffset = static_cast<uint32_t>(leftCapInset);
    uint32_t rightHorizontalOffset = static_cast<uint32_t>(width - rightCapInset);

    switch (resizingMode) {
        case xcassets::Resizing::Mode::ThreePartHorizontal:
            return {
                { leftHorizontalOffset,   0, leftWidth,   height },
                { centerHorizontalOffset, 0, centerWidth, height },
                { rightHorizontalOffset,  0, rightWidth,  height },
            };
        case xcassets::Resizing::Mode::ThreePartVertical:
            return {
                { 0, topVerticalOffset,    width, topHeight    },
                { 0, centerVerticalOffset, width, centerHeight },
                { 0, bottomVerticalOffset, width, bottomHeight },
            };
        case xcassets::Resizing::Mode::NinePart:
            return {
                { leftHorizontalOffset,   topVerticalOffset,    leftWidth,   topHeight    },
                { centerHorizontalOffset, topVerticalOffset,    centerWidth, topHeight    },
                { rightHorizontalOffset,  topVerticalOffset,    rightWidth,  topHeight    },
                { leftHorizontalOffset,   centerVerticalOffset, leftWidth,   centerHeight },
                { centerHorizontalOffset, centerVerticalOffset, centerWidth, centerHeight },
                { rightHorizontalOffset,  centerVerticalOffset, rightWidth,  centerHeight },
                { leftHorizontalOffset,   bottomVerticalOffset, leftWidth,   bottomHeight },
                { centerHorizontalOffset, bottomVerticalOffset, centerWidth, bottomHeight },
                { rightHorizontalOffset,  bottomVerticalOffset, rightWidth,  bottomHeight },
            };
        default: abort();
    }
}

bool ImageSet::
Compile(
    std::shared_ptr<xcassets::Asset::ImageSet> const &imageSet,
    Filesystem *filesystem,
    CompileOutput *compileOutput,
    Result *result)
{
    bool success = true;

    if (imageSet->images()) {
        for (xcassets::Asset::ImageSet::Image const &image : *imageSet->images()) {
            if (!CompileAsset(imageSet, image, filesystem, compileOutput, result)) {
                success = false;
            }
        }
    }

    return success;
}

static uint16_t
GenerateIdentifier(void) {
    static uint16_t last = 0;
    last = last + 1;
    return last;
}

bool ImageSet::
CompileAsset(
    std::shared_ptr<xcassets::Asset::ImageSet> const &imageSet,
    xcassets::Asset::ImageSet::Image const &image,
    Filesystem *filesystem,
    CompileOutput *compileOutput,
    Result *result)
{
    static std::map<std::string, uint16_t> idMap = {};

    /* Skip any entry that is not attached to a file, or is explicitly unassigned. */
    if (!image.fileName() || image.unassigned()) {
        return true;
    }

    /* An image without an idiom is considered unassigned. */
    if (!image.idiom()) {
        return false;
    }

    std::string filename = FSUtil::ResolveRelativePath(*image.fileName(), imageSet->path());

    std::string name = imageSet->name().string();

    /* The default (0) is any scale. */
    double scale = 0;
    if (image.scale()) {
        scale = image.scale()->value();
    }

    // TODO: filter by target-device / device-model / os-version
    uint16_t idiom = IdiomAttributeForIdiom(*image.idiom());

    std::vector<uint8_t> pixels;
    size_t width = 0;
    size_t height = 0;
    car::Rendition::Data::Format format;

    if (FSUtil::IsFileExtension(filename, "png", true)) {
        std::vector<uint8_t> contents;
        if (!filesystem->read(&contents, filename)) {
            result->normal(
                Result::Severity::Error,
                "unable to read PNG file",
                filename);
            return false;
        }

        auto png = graphics::Format::PNG::Read(contents);
        if (!png.first) {
            result->normal(Result::Severity::Error, png.second, filename);
            return false;
        }

        graphics::Image const &image = *png.first;
        width = image.width();
        height = image.height();

        /* Convert the image to the archive format. */
        switch (image.format().color()) {
            case graphics::PixelFormat::Color::RGB:
                format = car::Rendition::Data::Format::PremultipliedBGRA8;
                pixels = graphics::PixelFormat::Convert(
                    image.data(),
                    image.format(),
                    graphics::PixelFormat(
                        graphics::PixelFormat::Color::RGB,
                        graphics::PixelFormat::Order::Reversed,
                        graphics::PixelFormat::Alpha::PremultipliedFirst));
                break;
            case graphics::PixelFormat::Color::Grayscale:
                format = car::Rendition::Data::Format::PremultipliedGA8;
                pixels = graphics::PixelFormat::Convert(
                    image.data(),
                    image.format(),
                    graphics::PixelFormat(
                        graphics::PixelFormat::Color::Grayscale,
                        graphics::PixelFormat::Order::Reversed,
                        graphics::PixelFormat::Alpha::PremultipliedFirst));
                break;
        }
    } else if (FSUtil::IsFileExtension(filename, "jpg", true) || FSUtil::IsFileExtension(filename, "jpeg", true)) {
        if (!filesystem->read(&pixels, filename)) {
            result->normal(
                Result::Severity::Error,
                "unable to read JPEG file",
                filename);
            return false;
        }

        format = car::Rendition::Data::Format::JPEG;
    } else {
        result->normal(
            Result::Severity::Error,
            "unknown file type",
            filename);
        return false;
    }

    bool createFacet = false;
    uint16_t facetIdentifier = 0;
    auto it = idMap.find(name);
    if (it == idMap.end()) {
        facetIdentifier = GenerateIdentifier();
        idMap[name] = facetIdentifier;
        createFacet = true;
    } else {
        facetIdentifier = it->second;
    }
    if (createFacet) {
        car::AttributeList attributes = car::AttributeList({
            { car_attribute_identifier_identifier, facetIdentifier },
        });

        car::Facet facet = car::Facet::Create(name, attributes);
        compileOutput->car()->addFacet(facet);
    }

    /*
     * Create rendition for the image.
     */
    car::AttributeList attributes = car::AttributeList({
        { car_attribute_identifier_idiom, idiom },
        { car_attribute_identifier_scale, static_cast<int>(scale) },
        { car_attribute_identifier_identifier, facetIdentifier },
    });

    auto data = ext::optional<car::Rendition::Data>(car::Rendition::Data(std::move(pixels), format));

    car::Rendition rendition = car::Rendition::Create(attributes, std::move(data));
    rendition.width() = width;
    rendition.height() = height;
    rendition.scale() = scale;
    rendition.fileName() = *image.fileName();

    if (image.resizing()) {
        xcassets::Resizing const &resizing = *image.resizing();

        xcassets::Resizing::Center::Mode centerMode = xcassets::Resizing::Center::Mode::Tile;
        if (resizing.center()) {
            xcassets::Resizing::Center const &center = *resizing.center();
            if (center.mode()) {
                centerMode = *center.mode();
            }

            /* TODO: center size is currently ingnored */
        }

        if (resizing.mode()) {
            xcassets::Resizing::Mode resizingMode = *resizing.mode();
            rendition.layout() = LayoutForResizingAndCenterMode(resizingMode, centerMode);
            rendition.slices() = SlicesForResizingModeAndCapInsets(width, height, resizingMode, resizing.capInsets());
        }
    }

    compileOutput->car()->addRendition(std::move(rendition));
    return true;
}
