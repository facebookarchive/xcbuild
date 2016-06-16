/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <bom/bom.h>
#include <car/Reader.h>
#include <car/Facet.h>
#include <car/Rendition.h>

#include <iterator>
#include <string>
#include <fstream>

#include <cassert>
#include <cstring>

#include <arpa/inet.h>
#include <zlib.h>

static ext::optional<std::vector<uint8_t>>
png_encode(uint32_t width, uint32_t height, int depth, std::vector<uint8_t> const &data)
{
    std::vector<uint8_t> png;

    uint32_t const crc32_initial = crc32(0, NULL, 0);
    uint32_t crc32_big;
    uint8_t *crc32p = reinterpret_cast<uint8_t *>(&crc32_big);

    uint8_t const header[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
    png.insert(png.end(), std::begin(header), std::end(header));

    uint32_t width_big = htonl(width);
    uint8_t *width_buf = reinterpret_cast<uint8_t *>(&width_big);

    uint32_t height_big = htonl(height);
    uint8_t *height_buf = reinterpret_cast<uint8_t *>(&height_big);

    uint8_t const color_table[] = {
        [0] = UINT8_MAX,
        [1] = 0x00,
        [2] = 0x04,
        [3] = 0x02,
        [4] = 0x06,
    };
    if (depth > sizeof(color_table) || color_table[depth] == UINT8_MAX) {
        fprintf(stderr, "error: unsupported depth: %x\n", depth);
        return ext::nullopt;
    }

    uint8_t const ihdr[] = {
        0x0, 0x0, 0x0, 0xD, // chunk length
        'I', 'H', 'D', 'R', // chunk type
        width_buf[0], width_buf[1], width_buf[2], width_buf[3], // width_buf
        height_buf[0], height_buf[1], height_buf[2], height_buf[3], // height_buf
        0x8, // bit depth
        color_table[depth], // color type
        0x0, // compression method
        0x0, // filter method
        0x0, // interlace method
    };
    png.insert(png.end(), std::begin(ihdr), std::end(ihdr));

    crc32_big = crc32(crc32_initial, ihdr + 4, sizeof(ihdr) - 4);
    crc32_big = htonl(crc32_big);
    uint8_t const ihdr_crc32[] = { crc32p[0], crc32p[1], crc32p[2], crc32p[3] };
    png.insert(png.end(), std::begin(ihdr_crc32), std::end(ihdr_crc32));

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, 8, 15, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        fprintf(stderr, "error: deflate init failed\n");
        return ext::nullopt;
    }

    uint32_t filter_stride = width * depth;
    size_t filter_size = (data.size() / filter_stride);
    std::vector<uint8_t> buffer = std::vector<uint8_t>(data.size() + filter_size);
    for (size_t i = 0; i < height; ++i) {
        size_t offset = (i * filter_stride);
        size_t filter_offset = i;

        *(buffer.data() + offset + filter_offset) = 0; // filter format
        memcpy(buffer.data() + offset + filter_offset + 1, data.data() + offset, filter_stride);
    }

    strm.avail_in = buffer.size();
    strm.next_in = buffer.data();

    /* Maximum compressed size is x1.01 + 12 the uncompressed size. */
    std::vector<uint8_t> compressed = std::vector<uint8_t>(buffer.size() * 1.01 + 12);

    do {
        strm.avail_out = compressed.size() - strm.total_out;
        strm.next_out = (Bytef *)(compressed.data() + strm.total_out);

        ret = deflate(&strm, Z_FINISH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            fprintf(stderr, "error: deflate failed %d\n", ret);
            return ext::nullopt;
        }
    } while (ret != Z_STREAM_END);

    /* Shrink down to compressed size. */
    compressed.resize(strm.total_out);

    ret = deflateEnd(&strm);
    if (ret != Z_OK) {
        fprintf(stderr, "error: deflate end failed\n");
        return ext::nullopt;
    }

    uint32_t size_big = htonl(compressed.size());
    uint8_t *size_buf = reinterpret_cast<uint8_t *>(&size_big);

    uint8_t const idat[] = {
        size_buf[0], size_buf[1], size_buf[2], size_buf[3], // chunk length
        'I', 'D', 'A', 'T', // chunk type
    };
    png.insert(png.end(), std::begin(idat), std::end(idat));
    png.insert(png.end(), compressed.begin(), compressed.end());

    crc32_big = crc32(crc32_initial, idat + 4, sizeof(idat) - 4);
    crc32_big = crc32(crc32_big, compressed.data(), compressed.size());
    crc32_big = htonl(crc32_big);
    uint8_t const idat_crc32[] = { crc32p[0], crc32p[1], crc32p[2], crc32p[3] };
    png.insert(png.end(), std::begin(idat_crc32), std::end(idat_crc32));

    uint8_t const iend[] = {
        0x0, 0x0, 0x0, 0x0, // chunk length
        'I', 'E', 'N', 'D', // chunk type
    };
    png.insert(png.end(), std::begin(iend), std::end(iend));

    crc32_big = crc32(crc32_initial, iend + 4, sizeof(iend) - 4);
    crc32_big = htonl(crc32_big);
    uint8_t const iend_crc32[] = { crc32p[0], crc32p[1], crc32p[2], crc32p[3] };
    png.insert(png.end(), std::begin(iend_crc32), std::end(iend_crc32));

    return png;
}

static void
rendition_dump(car::Rendition const &rendition, std::string const &path)
{
    ext::optional<car::Rendition::Data> data = rendition.data();
    if (data) {
        uint32_t depth = car::Rendition::Data::FormatSize(data->format());
        std::vector<uint8_t> buffer = data->data();

        /* Unpremultiply alpha. */
        switch (data->format()) {
            case car::Rendition::Data::Format::PremultipliedBGRA8:
                for (size_t j = 0; j < buffer.size(); j += depth) {
                    /* Swizzle byte order to match PNG expectation. */
                    float b = buffer[j + 0] / 255.0;
                    float g = buffer[j + 1] / 255.0;
                    float r = buffer[j + 2] / 255.0;
                    float a = buffer[j + 3] / 255.0;
                    buffer[j + 0] = (a ? r / a : 0) * 255.0;
                    buffer[j + 1] = (a ? g / a : 0) * 255.0;
                    buffer[j + 2] = (a ? b / a : 0) * 255.0;
                    buffer[j + 3] = a * 255;
                }
                break;
            case car::Rendition::Data::Format::PremultipliedGA8:
                for (size_t j = 0; j < buffer.size(); j += depth) {
                    float g = buffer[j + 0] / 255.0;
                    float a = buffer[j + 1] / 255.0;
                    buffer[j + 0] = (a ? g / a : 0) * 255.0;
                    buffer[j + 1] = a * 255;
                }
                break;

            case car::Rendition::Data::Format::JPEG:
            case car::Rendition::Data::Format::Data:
                {
                    std::ofstream file;
                    file.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
                    if (file.fail()) {
                        fprintf(stderr, "error: failed to open file\n");
                        return;
                    }
                    std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(file));
                    file.close();
                    return;
                }
            default:
                fprintf(stderr, "error: unknown color format\n");
                return;
        }

        ext::optional<std::vector<uint8_t>> png = png_encode(rendition.width(), rendition.height(), depth, buffer);
        if (png) {
            std::ofstream file;
            file.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
            if (file.fail()) {
                fprintf(stderr, "error: failed to open file\n");
                return;
            }

            std::copy(png->begin(), png->end(), std::ostream_iterator<char>(file));
        }
    } else {
        printf("warning: failed to get image data for rendition\n");
    }
}

int
main(int argc, char **argv)
{
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "error: missing input\n");
        return 1;
    }

    std::string output = ".";
    if (argc > 2) {
        output = argv[2];
    }

    struct bom_context_memory memory = bom_context_memory_file(argv[1], false, 0);
    auto bom = std::unique_ptr<struct bom_context, decltype(&bom_free)>(bom_alloc_load(memory), bom_free);
    if (bom == nullptr) {
        fprintf(stderr, "error: unable to load BOM\n");
        return 1;
    }

    bom_variable_iterate(bom.get(), [](struct bom_context *context, const char *name, int data_index, void *ctx) {
        size_t data_len;
        void *data = bom_index_get(context, data_index, &data_len);
        (void)data;

        printf("Variable: %s [%08zx]\n", name, data_len);
        return true;
    }, NULL);
    printf("\n");

    ext::optional<car::Reader> car = car::Reader::Load(std::move(bom));
    if (!car) {
        fprintf(stderr, "error: unable to load car archive\n");
        return 1;
    }

    car->dump();
    printf("\n");

    int facet_count = 0;
    int rendition_count = 0;

    car->facetIterate([&car, &facet_count, &rendition_count, output](car::Facet const &facet) {
        facet_count++;
        facet.dump();

        auto renditions = car->lookupRenditions(facet);
        for (auto const &rendition : renditions) {
            rendition.dump();
            rendition_dump(rendition, output + "/" + rendition.fileName());
            rendition_count++;
        }
    });

    printf("Found %d facets and %d renditions\n", facet_count, rendition_count);
    return 0;
}

