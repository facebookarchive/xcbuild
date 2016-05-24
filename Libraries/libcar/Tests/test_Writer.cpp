/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <bom/bom_format.h>
#include <bom/bom.h>
#include <car/car_format.h>
#include <car/Facet.h>
#include <car/AttributeList.h>
#include <car/Rendition.h>
#include <car/Facet.h>
#include <car/Writer.h>
#include <car/Reader.h>

#include <cstdio>
#include <string>

#include <vector>

// Test pattern as raw pixed data, in PremultipliedBGRA8 format
static std::vector<uint8_t> test_pixels = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x7f, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct test_car_key_format {
    struct car_key_format keyfmt;
    uint32_t identifier_list[13];
} __attribute__((packed));

static test_car_key_format keyfmt_s = {
    {
        {'k', 'f', 'm', 't'}, 0, 13
    },
    {
        car_attribute_identifier_scale,
        car_attribute_identifier_idiom,
        car_attribute_identifier_subtype,
        car_attribute_identifier_graphics_class,
        car_attribute_identifier_memory_class,
        car_attribute_identifier_size_class_horizontal,
        car_attribute_identifier_size_class_vertical,
        car_attribute_identifier_identifier,
        car_attribute_identifier_element,
        car_attribute_identifier_part,
        car_attribute_identifier_state,
        car_attribute_identifier_value,
        car_attribute_identifier_dimension1
    }
};

static struct car_key_format *keyfmt = &keyfmt_s.keyfmt;

TEST(Writer, TestWriter)
{
    std::string tmpfilename = std::string("/tmp/test_car_Writer_output.car");
    int width = 8;
    int height = 8;

    // Write to test tmpfilename
    {
        struct bom_context_memory memory = bom_context_memory_file(tmpfilename.c_str(), true, sizeof(struct bom_header));
        auto bom = car::Writer::unique_ptr_bom(bom_alloc_empty(memory), bom_free);
        EXPECT_TRUE(bom != nullptr);

        auto writer = car::Writer::Create(std::move(bom));
        EXPECT_TRUE(writer != ext::nullopt);

        // Add key format
        writer->keyfmt() = keyfmt;

        car::AttributeList attributes = car::AttributeList({
            {car_attribute_identifier_idiom, car_attribute_identifier_idiom_value_universal},
            {car_attribute_identifier_scale, 2},
            {car_attribute_identifier_identifier, 1},
        });

        car::Rendition::Data::Format format = car::Rendition::Data::Format::PremultipliedBGRA8;
        auto data = ext::optional<car::Rendition::Data>(car::Rendition::Data::Data(test_pixels, format));
        car::Rendition rendition = car::Rendition::Create(attributes, data);
        rendition.width() = width;
        rendition.height() = height;
        rendition.scale() = 2;
        rendition.fileName() = std::string("testpattern.png");
        rendition.layout() = car_rendition_value_layout_one_part_scale;

        car::Facet facet = car::Facet::Create(std::string("testpattern"), attributes);

        writer->addFacet(facet);
        writer->addRendition(rendition);
        writer->Write();
    }

    // Read back from test tmpfilename
    {
        struct bom_context_memory memory = bom_context_memory_file(tmpfilename.c_str(), false, 0);
        auto bom = std::unique_ptr<struct bom_context, decltype(&bom_free)>(bom_alloc_load(memory), bom_free);
        EXPECT_TRUE(bom != nullptr);

        ext::optional<car::Reader> reader = car::Reader::Load(std::move(bom));
        EXPECT_TRUE(reader != ext::nullopt);

        int facet_count = 0;
        int rendition_count = 0;

        reader->facetIterate([&reader, &facet_count, &rendition_count](car::Facet const &facet) {
            facet_count++;

            EXPECT_TRUE(facet.name() == std::string("testpattern"));

            auto renditions = reader->lookupRenditions(facet);
            for (auto & rendition : renditions) {
                rendition_count++;

                auto data = rendition.data()->data();
                EXPECT_TRUE(data == test_pixels);
            }
        });

        EXPECT_TRUE(facet_count == 1);
        EXPECT_TRUE(rendition_count == 1);
    }
}


