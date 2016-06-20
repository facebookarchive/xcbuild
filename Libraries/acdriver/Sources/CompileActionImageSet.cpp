/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/CompileActionImageSet.h>
#include <acdriver/CompileAction.h>
#include <acdriver/Result.h>
#include <xcassets/Asset/ImageSet.h>
#include <xcassets/Slot/Idiom.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <bom/bom.h>
#include <car/Reader.h>
#include <car/Writer.h>
#include <car/Facet.h>
#include <car/Rendition.h>
#include <bom/bom_format.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <string>

#include <png.h>

using acdriver::CompileAction;
using acdriver::CompileOutput;
using acdriver::Options;
using acdriver::Result;

using libutil::FSUtil;

static bool readPNGFile(const char *filename, std::vector<unsigned char> &pixels, size_t *width_out, size_t *height_out, size_t *channels_out, Result *result)
{
    char header[8];
    png_struct *png_struct_ptr;
    png_info *info_struct_ptr;

    int number_of_passes;
    png_uint_32 row_bytes;
    png_byte **row_pointers = NULL;

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        result->normal(
            Result::Severity::Error,
            "unable to open PNG file",
            std::string(filename));
        return false;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp((png_const_bytep)header, 0, 8)) {
        fclose(fp);
        result->normal(
            Result::Severity::Error,
            "file is not a PNG file",
            std::string(filename));
        return false;
    }

    png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_struct_ptr) {
        fclose(fp);
        result->normal(
            Result::Severity::Error,
            "png_create_read_struct returned error",
            std::string(filename));
        return false;
    }

    info_struct_ptr = png_create_info_struct(png_struct_ptr);
    if (!info_struct_ptr) {
        png_destroy_read_struct (&png_struct_ptr, NULL, NULL);
        fclose(fp);
        result->normal(
            Result::Severity::Error,
            "png_create_info_struct returned error",
            std::string(filename));
        return false;
    }

    if (setjmp(png_jmpbuf(png_struct_ptr))) {
        png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, NULL);
        fclose(fp);
        result->normal(
            Result::Severity::Error,
            "setjmp/png_jmpbuf returned error",
            std::string(filename));
        return false;
    }

    png_init_io(png_struct_ptr, fp);
    png_set_sig_bytes(png_struct_ptr, 8);
    png_read_info(png_struct_ptr, info_struct_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_method, compression_method, filter_method, channels;
    if (!png_get_IHDR (png_struct_ptr, info_struct_ptr,
        &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
        png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, NULL);
        fclose(fp);
        result->normal(
            Result::Severity::Error,
            "setjmp/png_jmpbuf returned error",
            std::string(filename));
        return false;
    }

    /* Convert pixels to RGBA or GA8 */
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_struct_ptr);
        png_set_bgr(png_struct_ptr);
        png_set_filler(png_struct_ptr, 0xff, PNG_FILLER_AFTER);        
    }

    if (color_type == PNG_COLOR_TYPE_GRAY &&
        bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_struct_ptr);
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_struct_ptr);
    }

    if (bit_depth < 8) {
        png_set_packing(png_struct_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        png_set_bgr(png_struct_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) {
        png_set_filler(png_struct_ptr, 0xff, PNG_FILLER_AFTER);        
    }

    png_set_alpha_mode(png_struct_ptr, PNG_ALPHA_PREMULTIPLIED, PNG_DEFAULT_sRGB);

    number_of_passes = png_set_interlace_handling(png_struct_ptr);
    (void) number_of_passes;

    /* Make transforms take effect */
    png_read_update_info(png_struct_ptr, info_struct_ptr);

    if (!png_get_IHDR (png_struct_ptr, info_struct_ptr,
        &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
        png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "png_get_IHDR returned error",
            std::string(filename));
        return false;
    }

    /* color_type will still show original format */
    switch(color_type) {
        case PNG_COLOR_TYPE_GRAY:
            /* This is transformed to GA8 */
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            channels = 2;
            break;
        case PNG_COLOR_TYPE_PALETTE:
        case PNG_COLOR_TYPE_RGB:
            /* These are transformed to RGBA */
        case PNG_COLOR_TYPE_RGB_ALPHA:
            channels = 4;
            break;
        default:
            png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, NULL);
            fclose(fp);
            result->normal(
                Result::Severity::Error,
                "Unhandled PNG color type");
            return false;
    }

    row_bytes = png_get_rowbytes (png_struct_ptr, info_struct_ptr);
    if (row_bytes != (width * (bit_depth/8) * channels)) {
        png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "Unable to transform PNG pixel data");
        return false;
    }

    row_pointers = (png_byte **) malloc (height * sizeof (png_bytep));
    if (row_pointers == NULL)
    {
        png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, NULL);
        result->normal(
            Result::Severity::Error,
            "Could not allocate memory");
        return false;
    }

    pixels = std::vector<unsigned char>(width * height * channels);
    unsigned char *bytes = static_cast<unsigned char *>(pixels.data());
    for (int row = 0; row < height; row++) {
        row_pointers[row] = bytes + (row * row_bytes);
    }
    png_read_image(png_struct_ptr, row_pointers);

    /* clean up */
    png_read_end (png_struct_ptr, info_struct_ptr);
    png_destroy_read_struct (&png_struct_ptr, &info_struct_ptr, (png_infopp) NULL);
    free(row_pointers);
    fclose(fp);

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
GenerateIdentifier(void) {
    static uint16_t last = 0;
    last = last + 1;
    return last;
}

bool
CompileAsset(
    std::shared_ptr<xcassets::Asset::Catalog> const &catalog,
    std::string ns,
    Options const &options,
    CompileOutput *compileOutput,
    Result *result,
    std::shared_ptr<xcassets::Asset::Asset> const &parent,
    xcassets::Asset::ImageSet::Image const &image)
{
    static std::map<std::string, uint16_t> idMap = {};
    std::string name = FSUtil::GetBaseNameWithoutExtension(parent->path());

    /* Skip any entry that is not attached to a file */
    if (!image.fileName()) {
        return true;
    }

    std::string filename = parent->path() + std::string("/") + *image.fileName();

    /* An image without an idiom is considered unassigned */
    if (!image.idiom()) {
        return false;
    }

    /* Add namespace to name */
    if (ns.size() > 0) {
        name = ns + std::string("/") + name;        
    }

    /* scale defaults to 0 / all */
    double scale = 0;
    if (image.scale()) {
        scale = image.scale()->value();
    }

    // TODO: We should filter by target-device / device-model / os-version
    uint16_t idiom = car_attribute_identifier_idiom_value_universal;
    switch(*image.idiom()) {
        case xcassets::Slot::Idiom::Universal:
            idiom = car_attribute_identifier_idiom_value_universal;
            break;
        case xcassets::Slot::Idiom::Phone:
            idiom = car_attribute_identifier_idiom_value_phone;
            break;
        case xcassets::Slot::Idiom::Pad:
            idiom = car_attribute_identifier_idiom_value_pad;
            break;
        case xcassets::Slot::Idiom::Desktop:
            // TODO: desktop has no idiom value
            break;
        case xcassets::Slot::Idiom::TV:
            idiom = car_attribute_identifier_idiom_value_tv;
            break;
        case xcassets::Slot::Idiom::Watch:
            idiom = car_attribute_identifier_idiom_value_watch;
            break;
    }

    std::vector<unsigned char> pixels;
    size_t width = 0;
    size_t height = 0;
    size_t channels = 0;
    car::Rendition::Data::Format format;

    std::string filenameLowerCase(filename.size(), '\0');
    std::transform(filename.begin(), filename.end(), filenameLowerCase.begin(), ::tolower);

    if (FSUtil::GetFileExtension(filenameLowerCase) == "png") {
        if(!readPNGFile(filename.c_str(), pixels, &width, &height, &channels, result)) {
            return false;
        }
        format = channels == 4 ?
        car::Rendition::Data::Format::PremultipliedBGRA8 :
        car::Rendition::Data::Format::PremultipliedGA8;
    } else if (FSUtil::GetFileExtension(filenameLowerCase) == "jpg" ||
        FSUtil::GetFileExtension(filenameLowerCase) == "jpeg") {
        std::ifstream inputFile(filename.c_str(), std::ios::binary);
        if (inputFile.is_open()) {
            inputFile.seekg(0, std::ios::end);   
            pixels.reserve(inputFile.tellg());
            inputFile.seekg(0, std::ios::beg);
            pixels.assign((std::istreambuf_iterator<char>(inputFile)),
                          std::istreambuf_iterator<char>());
            format = car::Rendition::Data::Format::JPEG;
        } else {
            result->normal(
                Result::Severity::Error,
                "unable to open JPG file",
                std::string(filename));
            return false;
        }
    } else {
        result->normal(
            Result::Severity::Error,
            "unable to load file",
            std::string(filename));
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
            { car_attribute_identifier_identifier, facetIdentifier }
        });

        car::Facet facet = car::Facet::Create(name, attributes);
        compileOutput->car()->addFacet(facet);
    }

    car::AttributeList attributes = car::AttributeList({
        { car_attribute_identifier_idiom, idiom },
        { car_attribute_identifier_scale, (int)scale },
        { car_attribute_identifier_identifier, facetIdentifier },
    });

    auto data = ext::optional<car::Rendition::Data>(car::Rendition::Data(std::move(pixels), format));

    car::Rendition rendition = car::Rendition::Create(attributes, std::move(data));

    rendition.width() = width;
    rendition.height() = height;
    rendition.scale() = scale;
    rendition.fileName() = *image.fileName();

    if (image.resizing()) {
        auto resizing = *image.resizing();
        double topCapInset = 0;
        double leftCapInset = 0;
        double bottomCapInset = 0;
        double rightCapInset = 0;
        if (resizing.capInsets()) {
            auto capInsets = *resizing.capInsets();
            topCapInset = *capInsets.top();
            leftCapInset = *capInsets.left();
            bottomCapInset = *capInsets.bottom();
            rightCapInset = *capInsets.right();
        }

        xcassets::Resizing::Center::Mode centreMode = xcassets::Resizing::Center::Mode::Tile;
        if (resizing.center()) {
            auto center = *resizing.center();
            if (center.mode()) {
                centreMode = *center.mode();
            }

            /* TODO: center size is currently ingnored */
        }

        std::vector<car::Rendition::Slice> slices;
        enum car_rendition_value_layout layout = car_rendition_value_layout_one_part_fixed_size;
        switch(*resizing.mode()) {
            case xcassets::Resizing::Mode::ThreePartHorizontal:
                {
                    slices.push_back({0, 0, (uint32_t)leftCapInset, (uint32_t)height});
                    slices.push_back({(uint32_t)leftCapInset, 0, (uint32_t)(width - (leftCapInset + rightCapInset)), (uint32_t)height});
                    slices.push_back({(uint32_t)(width - rightCapInset), 0, (uint32_t)rightCapInset, (uint32_t)height});

                    switch(centreMode) {
                        case xcassets::Resizing::Center::Mode::Tile:
                            layout = car_rendition_value_layout_three_part_horizontal_tile;
                            break;
                        case xcassets::Resizing::Center::Mode::Stretch:
                            layout = car_rendition_value_layout_three_part_horizontal_scale;
                            break;
                        default:
                            layout = car_rendition_value_layout_three_part_horizontal_tile;
                    }
                }
                break;

            case xcassets::Resizing::Mode::ThreePartVertical:
                {
                    slices.push_back({0, (uint32_t)(height - topCapInset), (uint32_t)width, (uint32_t)topCapInset});
                    slices.push_back({0, (uint32_t)bottomCapInset, (uint32_t)width, (uint32_t)(height - (topCapInset + bottomCapInset))});
                    slices.push_back({0, 0, (uint32_t)width, (uint32_t)bottomCapInset});

                    switch(centreMode) {
                        case xcassets::Resizing::Center::Mode::Tile:
                            layout = car_rendition_value_layout_three_part_vertical_tile;
                            break;
                        case xcassets::Resizing::Center::Mode::Stretch:
                            layout = car_rendition_value_layout_three_part_vertical_scale;
                            break;
                        default:
                            layout = car_rendition_value_layout_three_part_vertical_tile;
                    }
                }
                break;

            case xcassets::Resizing::Mode::NinePart:
                {
                    uint32_t centerWidth = width - (leftCapInset + rightCapInset);
                    uint32_t centerHeight = height - (topCapInset + bottomCapInset);

                    // each slice is the origin and size of a resizeable part of the image
                    slices = std::vector<car::Rendition::Slice>(9);

                    // first column
                    for (auto i : {0,3,6}) {
                        slices[i].x = 0;
                        slices[i].width = leftCapInset;
                    }
                    // second column
                    for (auto i : {1,4,7}) {
                        slices[i].x = leftCapInset;
                        slices[i].width = centerWidth; 
                    }
                    // third column
                    for (auto i : {2,5,8}) {
                        slices[i].x = width - rightCapInset;
                        slices[i].width = rightCapInset;
                    }
                    // first row
                    for (auto i : {0,1,2}) {
                        slices[i].y = height - topCapInset;
                        slices[i].height = topCapInset;
                    }
                    // second row
                    for (auto i : {3,4,5}) {
                        slices[i].y = bottomCapInset;
                        slices[i].height = centerHeight;
                    }
                    // third row
                    for (auto i : {6,7,8}) {
                        slices[i].y = 0;
                        slices[i].height = bottomCapInset;
                    }

                    switch(centreMode) {
                        case xcassets::Resizing::Center::Mode::Tile:
                            layout = car_rendition_value_layout_nine_part_tile;
                            break;
                        case xcassets::Resizing::Center::Mode::Stretch:
                            layout = car_rendition_value_layout_nine_part_scale;
                            break;
                        default:
                            layout = car_rendition_value_layout_nine_part_tile;
                    }
                }
                break;
            default:
                printf("Resizing:Mode:Unknown\n");
        }
        rendition.slices() = slices;
        rendition.layout() = layout;
    }

    compileOutput->car()->addRendition(std::move(rendition));
    return true;
}
