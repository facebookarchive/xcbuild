/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <car/Rendition.h>
#include <car/Reader.h>
#include <car/car_format.h>

#include <cassert>
#include <cstring>
#include <cstdio>

#include <zlib.h>

#if defined(__APPLE__)
#include <Availability.h>
#include <TargetConditionals.h>
#if (TARGET_OS_MAC && __MAC_10_11 && __MAC_OS_X_VERSION_MIN_REQUIRED > __MAC_10_11) || (TARGET_OS_IPHONE && __IPHONE_9_0 && __IPHONE_OS_VERSION_MIN_REQUIRED > __IPHONE_9_0)
#define HAVE_LIBCOMPRESSION 1
#endif
#endif

#if HAVE_LIBCOMPRESSION
#include <compression.h>
#define _COMPRESSION_LZVN 0x900
#endif

using car::Rendition;

Rendition::Data::
Data(std::vector<uint8_t> const &data, Format format) :
    _data  (data),
    _format(format)
{
}

size_t Rendition::Data::
FormatSize(Rendition::Data::Format format)
{
    switch (format) {
        case Format::PremultipliedBGRA8:
            return 4;
        case Format::PremultipliedGA8:
            return 2;
        case Format::Data:
            return 1;
    }

    abort();
}

Rendition::
Rendition(AttributeList const &attributes, std::function<ext::optional<Data>(Rendition const *)> const &data) :
    _attributes(attributes),
    _data      (data),
    _width     (0),
    _height    (0),
    _scale     (1.0)
{
}

void Rendition::
dump() const
{
    printf("Rendition: %s\n", _fileName.c_str());
    printf("Width: %d\n", _width);
    printf("Height: %d\n", _height);
    printf("Scale: %f\n", _scale);
    _attributes.dump();
}

static ext::optional<Rendition::Data> _decode(struct car_rendition_value *value);

Rendition const Rendition::
Load(
    AttributeList const &attributes,
    struct car_rendition_value *value)
{
    Rendition rendition = Rendition(attributes, [value](Rendition const *rendition) -> ext::optional<Data> {
        return _decode(value);
    });

    rendition.fileName() = std::string(value->metadata.name, sizeof(value->metadata.name));
    rendition.width() = value->width;
    rendition.height() = value->height;
    rendition.scale() = (float)value->scale_factor / 100.0;

    return rendition;
}

ext::optional<Rendition::Data> Rendition::
decode() const
{
    return _data(this);
}

static ext::optional<Rendition::Data>
_decode(struct car_rendition_value *value)
{
    struct car_rendition_info_header *info_header = (struct car_rendition_info_header *)value->info;
    while (((uintptr_t)info_header - (uintptr_t)value->info) < value->info_len) {
        info_header = (struct car_rendition_info_header *)((intptr_t)info_header + sizeof(struct car_rendition_info_header) + info_header->length);
    }

    Rendition::Data::Format format;
    if (value->pixel_format == car_rendition_value_pixel_format_argb) {
        format = Rendition::Data::Format::PremultipliedBGRA8;
    } else if (value->pixel_format == car_rendition_value_pixel_format_ga8) {
        format = Rendition::Data::Format::PremultipliedGA8;
    } else {
        format = Rendition::Data::Format::Data;
        fprintf(stderr, "error: unsupported pixel format %.4s\n", (char const *)&value->pixel_format);
        return ext::nullopt;
    }

    size_t bytes_per_pixel = Rendition::Data::FormatSize(format);
    size_t uncompressed_length = value->width * value->height * bytes_per_pixel;
    Rendition::Data data = Rendition::Data(std::vector<uint8_t>(uncompressed_length), format);
    void *uncompressed_data = static_cast<void *>(data.data().data());

    /* Advance past the header and the info section. We just want the data. */
    struct car_rendition_data_header1 *header1 = (struct car_rendition_data_header1 *)((uintptr_t)value + sizeof(struct car_rendition_value) + value->info_len);

    if (strncmp(header1->magic, "MLEC", sizeof(header1->magic)) != 0) {
        fprintf(stderr, "error: header1 magic is wrong, can't possibly decode\n");
        return ext::nullopt;
    }

    void *compressed_data = &header1->data;
    size_t compressed_length = header1->length;

    /* Check for the secondary header, and use its values if available. */
    /* todo find a way of determining in advance if this is present */
    struct car_rendition_data_header2 *header2 = (struct car_rendition_data_header2 *)compressed_data;
    if (strncmp(header2->magic, "KCBC", 4) == 0) {
        compressed_data = &header2->data;
        compressed_length = header2->length;
    }

    size_t offset = 0;
    while (offset < uncompressed_length) {
        if (offset != 0) {
            struct car_rendition_data_header2 *header2 = (struct car_rendition_data_header2 *)compressed_data;
            assert(strncmp(header2->magic, "KCBC", sizeof(header2->magic)) == 0);
            compressed_length = header2->length;
            compressed_data = header2->data;
        }

        if (header1->compression == car_rendition_data_compression_magic_zlib) {
            z_stream strm;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            strm.avail_in = compressed_length;
            strm.next_in = (Bytef *)compressed_data;

            int ret = inflateInit2(&strm, 16+MAX_WBITS);
            if (ret != Z_OK) {
               return ext::nullopt;
            }

            strm.avail_out = uncompressed_length;
            strm.next_out = (Bytef *)uncompressed_data;

            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                printf("error: decompression failure: %x.\n", ret);
                return ext::nullopt;
            }

            ret = inflateEnd(&strm);
            if (ret != Z_OK) {
                return ext::nullopt;
            }

            offset += (uncompressed_length - strm.avail_out);
        } else if (header1->compression == car_rendition_data_compression_magic_rle) {
            fprintf(stderr, "error: unable to handle RLE\n");
            return ext::nullopt;
        } else if (header1->compression == car_rendition_data_compression_magic_unk1) {
            fprintf(stderr, "error: unable to handle UNKNOWN\n");
            return ext::nullopt;
        } else if (header1->compression == car_rendition_data_compression_magic_lzvn || header1->compression == car_rendition_data_compression_magic_jpeg_lzfse) {
#if HAVE_LIBCOMPRESSION
            compression_algorithm algorithm;
            if (header1->compression == car_rendition_data_compression_magic_lzvn) {
                algorithm = (compression_algorithm)_COMPRESSION_LZVN;
            } else if (header1->compression == car_rendition_data_compression_magic_jpeg_lzfse) {
                algorithm = COMPRESSION_LZFSE;
            } else {
                assert(false);
            }

            size_t compression_result = compression_decode_buffer((uint8_t *)uncompressed_data + offset, uncompressed_length - offset, (uint8_t *)compressed_data, compressed_length, NULL, algorithm);
            if (compression_result != 0) {
                offset += compression_result;
                compressed_data = (void *)((uintptr_t)compressed_data + compressed_length);
            } else {
                fprintf(stderr, "error: decompression failure\n");
                return ext::nullopt;
            }
#else
            if (header1->compression == car_rendition_data_compression_magic_lzvn) {
                fprintf(stderr, "error: unable to handle LZVN\n");
                return ext::nullopt;
            } else if (header1->compression == car_rendition_data_compression_magic_jpeg_lzfse) {
                fprintf(stderr, "error: unable to handle LZFSE\n");
                return ext::nullopt;
            } else {
                assert(false);
            }
#endif
        } else if (header1->compression == car_rendition_data_compression_magic_blurredimage) {
            fprintf(stderr, "error: unable to handle BlurredImage\n");
            return ext::nullopt;
        } else {
            fprintf(stderr, "error: unknown compression algorithm %x\n", header1->compression);
            return ext::nullopt;
        }
    }

    return data;
}

Rendition Rendition::
Create(
    AttributeList const &attributes,
    std::function<ext::optional<Data>(Rendition const *)> const &data)
{
    return Rendition(attributes, data);
}

