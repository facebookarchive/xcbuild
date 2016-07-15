/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <graphics/Format/PNG.h>

#include <ext/optional>

using graphics::Format::PNG;
using graphics::Image;
using graphics::PixelFormat;

#if defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

/*
 * Smart pointer for CoreFoundation types.
 */
template<typename CF>
class CFHandleDeleter
{
public:
    void operator()(CF_CONSUMED CF object)
    {
        if (object != NULL) {
            CFRelease(object);
        }
    }
};

template<typename CF>
using CFHandle = std::unique_ptr<typename std::remove_pointer<CF>::type, CFHandleDeleter<CF>>;

static ext::optional<PixelFormat>
PixelFormatFromCGColorSpaceAndCGBitmapInfo(CGColorSpaceRef colorSpace, CGBitmapInfo bitmapInfo)
{
    /* Float components are not supported. */
    if ((bitmapInfo & kCGBitmapFloatComponents) != 0) {
        return ext::nullopt;
    }

    /* Convert color space. */
    PixelFormat::Color color;
    switch (CGColorSpaceGetModel(colorSpace)) {
        case kCGColorSpaceModelMonochrome:
            color = PixelFormat::Color::Grayscale;
            break;
        case kCGColorSpaceModelRGB:
            color = PixelFormat::Color::RGB;
            break;
        default:
            return ext::nullopt;
    }

    /* Convert byte order. */
    PixelFormat::Order order;
    switch (bitmapInfo & kCGBitmapByteOrderMask) {
        case kCGBitmapByteOrderDefault:
        case kCGBitmapByteOrder16Big:
        case kCGBitmapByteOrder32Big:
            order = PixelFormat::Order::Forward;
            break;
        case kCGBitmapByteOrder16Little:
        case kCGBitmapByteOrder32Little:
            order = PixelFormat::Order::Reversed;
            break;
        default:
            return ext::nullopt;
    }

    /* Convert alpha format. */
    PixelFormat::Alpha alpha;
    switch (bitmapInfo & kCGBitmapAlphaInfoMask) {
        case kCGImageAlphaNone:
            alpha = PixelFormat::Alpha::None;
            break;
        case kCGImageAlphaFirst:
            alpha = PixelFormat::Alpha::First;
            break;
        case kCGImageAlphaLast:
            alpha = PixelFormat::Alpha::Last;
            break;
        case kCGImageAlphaPremultipliedFirst:
            alpha = PixelFormat::Alpha::PremultipliedFirst;
            break;
        case kCGImageAlphaPremultipliedLast:
            alpha = PixelFormat::Alpha::PremultipliedLast;
            break;
        case kCGImageAlphaNoneSkipFirst:
            alpha = PixelFormat::Alpha::IgnoredFirst;
            break;
        case kCGImageAlphaNoneSkipLast:
            alpha = PixelFormat::Alpha::IgnoredLast;
            break;
        default:
            return ext::nullopt;
    }

    return PixelFormat(color, order, alpha);
}

std::pair<ext::optional<Image>, std::string> PNG::
Read(std::vector<uint8_t> const &contents)
{
    /*
     * Load the image.
     */
    auto data = CFHandle<CFDataRef>(CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, contents.data(), contents.size(), kCFAllocatorNull));
    if (data == NULL) {
        return std::make_pair(ext::nullopt, "unable to create data");
    }

    auto options = CFHandle<CFDictionaryRef>(CFDictionaryCreate(kCFAllocatorDefault, NULL, NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
    if (options == NULL) {
        return std::make_pair(ext::nullopt, "unable to create options");
    }

    auto imageSource = CFHandle<CGImageSourceRef>(CGImageSourceCreateWithData(data.get(), options.get()));
    if (imageSource == NULL) {
        return std::make_pair(ext::nullopt, "unable to create image source");
    }

    auto image = CFHandle<CGImageRef>(CGImageSourceCreateImageAtIndex(imageSource.get(), 0, NULL));
    if (image == NULL) {
        return std::make_pair(ext::nullopt, "unable to create image");
    }

    /*
     * Determine image properties.
     */
    size_t width = CGImageGetWidth(image.get());
    size_t height = CGImageGetHeight(image.get());
    CGColorSpaceRef inputColorSpace = CGImageGetColorSpace(image.get());
    CGImageAlphaInfo inputAlphaInfo = CGImageGetAlphaInfo(image.get());
    CGBitmapInfo inputBitmapInfo = CGImageGetBitmapInfo(image.get());

    if (auto format = PixelFormatFromCGColorSpaceAndCGBitmapInfo(inputColorSpace, inputBitmapInfo | inputAlphaInfo)) {
        /*
         * Supported image format; use the image data as-is.
         */
        CGDataProviderRef dataProvider = CGImageGetDataProvider(image.get());
        if (dataProvider == NULL) {
            return std::make_pair(ext::nullopt, "unable to get data provider");
        }

        auto data = CFHandle<CFDataRef>(CGDataProviderCopyData(dataProvider));
        if (data == NULL) {
            return std::make_pair(ext::nullopt, "unable to copy data");
        }

        /* Create result buffer. */
        auto pixels = std::vector<uint8_t>(width * height * format->bytesPerPixel());
        if (CFDataGetLength(data.get()) != pixels.size()) {
            return std::make_pair(ext::nullopt, "data is of unexpected length");
        }

        /* Copy the image data to the result. */
        CFRange range = CFRangeMake(0, CFDataGetLength(data.get()));
        CFDataGetBytes(data.get(), range, pixels.data());

        Image image = Image(width, height, *format, pixels);
        return std::make_pair(image, std::string());
    } else {
        /*
         * Convert to a supported image format. Note that CoreGraphics does not support
         * gray + alpha bitmap contexts, so always convert here to full color with alpha.
         */
        auto colorSpace = CFHandle<CGColorSpaceRef>(CGColorSpaceCreateDeviceRGB());
        if (colorSpace == NULL) {
            return std::make_pair(ext::nullopt, "unable to create color space");
        }

        /* Create a context for the result. */
        size_t channels = 4;
        size_t bitsPerComponent = 8;
        size_t bytesPerRow = (width * channels);
        CGBitmapInfo bitmapInfo = (kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
        std::vector<uint8_t> backing = std::vector<uint8_t>(width * height * channels);
        auto bitmapContext = CFHandle<CGContextRef>(CGBitmapContextCreate(backing.data(), width, height, bitsPerComponent, bytesPerRow, colorSpace.get(), bitmapInfo));
        if (bitmapContext == NULL) {
            return std::make_pair(ext::nullopt, "unable to create bitmap context");
        }

        /* Draw the image into the context. */
        CGRect rect = CGRectMake(0, 0, width, height);
        CGContextDrawImage(bitmapContext.get(), rect, image.get());

        /*
         * If the input image is grayscale, convert the pixel data to grayscale.
         */
        if (CGColorSpaceGetModel(inputColorSpace) == kCGColorSpaceModelMonochrome) {
            /* Convert to grayscale. */
            auto intermediateFormat = *PixelFormatFromCGColorSpaceAndCGBitmapInfo(colorSpace.get(), bitmapInfo);
            auto format = PixelFormat(PixelFormat::Color::Grayscale, intermediateFormat.order(), intermediateFormat.alpha());
            std::vector<uint8_t> pixels = PixelFormat::Convert(backing, intermediateFormat, format);
            Image image = Image(width, height, format, pixels);
            return std::make_pair(image, std::string());
        } else {
            /* Not grayscale, return it as-is. */
            PixelFormat format = *PixelFormatFromCGColorSpaceAndCGBitmapInfo(colorSpace.get(), bitmapInfo);
            Image image = Image(width, height, format, backing);
            return std::make_pair(image, std::string());
        }
    }
}

#else

#include <png.h>
#include <string.h>

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

std::pair<ext::optional<Image>, std::string> PNG::
Read(std::vector<uint8_t> const &contents)
{
    if (contents.size() < 8 || png_sig_cmp(static_cast<png_const_bytep>(contents.data()), 0, 8)) {
        return std::make_pair(ext::nullopt, "contents is not a PNG");
    }

    png_struct *png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_struct_ptr == NULL) {
        return std::make_pair(ext::nullopt, "png_create_read_struct returned error");
    }

    png_info *info_struct_ptr = png_create_info_struct(png_struct_ptr);
    if (info_struct_ptr == NULL) {
        png_destroy_read_struct(&png_struct_ptr, NULL, NULL);
        return std::make_pair(ext::nullopt, "png_create_info_struct returned error");
    }

    if (setjmp(png_jmpbuf(png_struct_ptr))) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        return std::make_pair(ext::nullopt, "setjmp/png_jmpbuf returned error");
    }

    unsigned char const *contents_ptr = static_cast<unsigned char const *>(contents.data());
    png_set_read_fn(png_struct_ptr, &contents_ptr, png_user_read_data);

    png_read_info(png_struct_ptr, info_struct_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_method, compression_method, filter_method;
    if (!png_get_IHDR(png_struct_ptr, info_struct_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        return std::make_pair(ext::nullopt, "failed to read PNG header");
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

    /* Convert palleted images to RGB. */
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_struct_ptr);
    }

    /* Handle interlaced images. */
    (void)png_set_interlace_handling(png_struct_ptr);

    /* Apply transforms. */
    png_read_update_info(png_struct_ptr, info_struct_ptr);

    if (!png_get_IHDR(png_struct_ptr, info_struct_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method)) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        return std::make_pair(ext::nullopt, "png_get_IHDR returned error");
    }

    /* Determine output pixel format. */
    PixelFormat::Color color;
    PixelFormat::Alpha alpha;
    switch (color_type) {
        case PNG_COLOR_TYPE_RGB:
            color = PixelFormat::Color::RGB;
            alpha = PixelFormat::Alpha::None;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            color = PixelFormat::Color::RGB;
            alpha = PixelFormat::Alpha::Last;
            break;
        case PNG_COLOR_TYPE_GRAY:
            color = PixelFormat::Color::Grayscale;
            alpha = PixelFormat::Alpha::None;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            color = PixelFormat::Color::Grayscale;
            alpha = PixelFormat::Alpha::Last;
            break;
        case PNG_COLOR_TYPE_PALETTE: {
            /* Converted to RGB. */
            color = PixelFormat::Color::RGB;

            /* Alpha is separate for palleted images. */
            png_bytep trans_alpha = NULL;
            int num_trans = 0;
            png_color_16p trans_color = NULL;
            png_get_tRNS(png_struct_ptr, info_struct_ptr, &trans_alpha, &num_trans, &trans_color);
            alpha = (trans_alpha != NULL ? PixelFormat::Alpha::Last : PixelFormat::Alpha::None);
            break;
        }
        default:
            png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
            return std::make_pair(ext::nullopt, "unhandled PNG color type");
    }
    PixelFormat format = PixelFormat(color, PixelFormat::Order::Forward, alpha);

    png_uint_32 row_bytes = png_get_rowbytes(png_struct_ptr, info_struct_ptr);
    if (row_bytes != (width * (bit_depth / 8) * format.bytesPerPixel())) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        return std::make_pair(ext::nullopt, "unable to transform PNG pixel data");
    }

    png_byte **row_pointers = (png_byte **)malloc(height * sizeof(png_bytep));
    if (row_pointers == NULL) {
        png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, NULL);
        return std::make_pair(ext::nullopt, "could not allocate memory");
    }

    auto pixels = std::vector<uint8_t>(width * height * format.bytesPerPixel());
    unsigned char *bytes = static_cast<unsigned char *>(pixels.data());
    for (int row = 0; row < height; row++) {
        row_pointers[row] = bytes + (row * row_bytes);
    }
    png_read_image(png_struct_ptr, row_pointers);

    /* Clean up. */
    png_read_end(png_struct_ptr, info_struct_ptr);
    png_destroy_read_struct(&png_struct_ptr, &info_struct_ptr, (png_infopp)NULL);
    free(row_pointers);

    Image image = Image(width, height, format, pixels);
    return std::make_pair(image, std::string());
}

#endif
