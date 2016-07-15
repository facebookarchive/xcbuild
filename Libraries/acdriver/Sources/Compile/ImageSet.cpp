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

#include <png.h>
#include <string.h>

using acdriver::Compile::ImageSet;
using acdriver::CompileOutput;
using acdriver::Result;
using libutil::Filesystem;
using libutil::FSUtil;

static void
png_user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    unsigned char **contents_ptr = (unsigned char **)png_get_io_ptr(png_ptr);
    if (contents_ptr == NULL) {
        return;
    }

    memcpy(data, *contents_ptr, length);
    *contents_ptr += length;
}

static bool
ReadPNGFile(
    std::vector<unsigned char> &contents,
    std::string const &filename,
    std::vector<uint8_t> *pixels,
    size_t *width_out,
    size_t *height_out,
    size_t *channels_out,
    Result *result)
{
    if (contents.size() < 8 || png_sig_cmp(static_cast<png_const_bytep>(contents.data()), 0, 8)) {
        result->normal(
            Result::Severity::Error,
            "file is not a PNG file",
            filename);
        return false;
    }

    png_struct *png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_struct_ptr == NULL) {
        result->normal(
            Result::Severity::Error,
            "png_create_read_struct returned error",
            filename);
        return false;
    }

    png_info *info_struct_ptr = png_create_info_struct(png_struct_ptr);
    if (info_struct_ptr == NULL) {
        png_destroy_read_struct(&png_struct_ptr, NULL, NULL);
        result->normal(
            Result::Severity::Error,
            "png_create_info_struct returned error",
            filename);
        return false;
    }

    if (setjmp(png_jmpbuf(png_struct_ptr))) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "setjmp/png_jmpbuf returned error",
            filename);
        return false;
    }

    unsigned char *contents_ptr = static_cast<unsigned char *>(contents.data());
    png_set_read_fn(png_struct_ptr, &contents_ptr, png_user_read_data);

    png_read_info(png_struct_ptr, info_struct_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_method, compression_method, filter_method, channels;
    if (!png_get_IHDR(png_struct_ptr, info_struct_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "failed to read PNG header",
            filename);
        return false;
    }

    /* Convert to a standard bit depth. */
    if (bit_depth == 16) {
        png_set_strip_16(png_struct_ptr);
    } else if (bit_depth < 8) {
        if (color_type == PNG_COLOR_TYPE_GRAY) {
            png_set_expand_gray_1_2_4_to_8(png_struct_ptr);
        } else {
            png_set_packing(png_struct_ptr);
        }
    }

    /* Convert pixels to RGBA or GA8. */
    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_struct_ptr);
            png_set_bgr(png_struct_ptr);
            png_set_filler(png_struct_ptr, 0xff, PNG_FILLER_AFTER);
            break;
        case PNG_COLOR_TYPE_GRAY:
            png_set_filler(png_struct_ptr, 0xff, PNG_FILLER_AFTER);
            break;
        case PNG_COLOR_TYPE_RGB:
            png_set_bgr(png_struct_ptr);
            png_set_filler(png_struct_ptr, 0xff, PNG_FILLER_AFTER);
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            png_set_bgr(png_struct_ptr);
            break;
    }

    png_set_alpha_mode(png_struct_ptr, PNG_ALPHA_PREMULTIPLIED, PNG_DEFAULT_sRGB);

    /* Handle interlaced images. */
    (void)png_set_interlace_handling(png_struct_ptr);

    /* Apply transforms. */
    png_read_update_info(png_struct_ptr, info_struct_ptr);

    if (!png_get_IHDR(png_struct_ptr, info_struct_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "png_get_IHDR returned error",
            filename);
        return false;
    }

    /* color_type will still show original format */
    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            /* This is transformed to GA8. */
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            channels = 2;
            break;
        case PNG_COLOR_TYPE_PALETTE:
        case PNG_COLOR_TYPE_RGB:
            /* These are transformed to RGBA. */
        case PNG_COLOR_TYPE_RGB_ALPHA:
            channels = 4;
            break;
        default:
            png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
            result->normal(
                Result::Severity::Error,
                "Unhandled PNG color type");
            return false;
    }

    png_uint_32 row_bytes = png_get_rowbytes(png_struct_ptr, info_struct_ptr);
    if (row_bytes != (width * (bit_depth / 8) * channels)) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "Unable to transform PNG pixel data");
        return false;
    }

    png_byte **row_pointers = (png_byte **)malloc(height * sizeof(png_bytep));
    if (row_pointers == NULL) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "Could not allocate memory");
        return false;
    }

    *pixels = std::vector<uint8_t>(width * height * channels);
    unsigned char *bytes = static_cast<unsigned char *>(pixels->data());
    for (int row = 0; row < height; row++) {
        row_pointers[row] = bytes + (row * row_bytes);
    }
    png_read_image(png_struct_ptr, row_pointers);

    /* Clean up. */
    png_read_end(png_struct_ptr, info_struct_ptr);
    png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, (png_infopp)NULL);
    free(row_pointers);

    if (width_out) {
        *width_out = width;
    }
    if (height_out) {
        *height_out = height;
    }
    if (channels_out) {
        *channels_out = channels;
    }
    return true;
}

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
    size_t channels = 0;
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

        if (!ReadPNGFile(contents, filename, &pixels, &width, &height, &channels, result)) {
            return false;
        }

        if (channels == 4) {
            format = car::Rendition::Data::Format::PremultipliedBGRA8;
        } else {
            format = car::Rendition::Data::Format::PremultipliedGA8;
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
