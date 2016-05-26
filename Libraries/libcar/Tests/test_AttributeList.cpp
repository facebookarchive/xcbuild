/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <car/AttributeList.h>
#include <car/car_format.h>


struct test_car_key_format {
    struct car_key_format keyfmt;
    uint32_t identifier_list[13];
} __attribute__((packed));

static struct test_car_key_format keyfmt_s = {
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


TEST(AttributeList, TestAttributeListDeSerialize)
{
    uint16_t rendition_key[13] = {
        1,                                        // scale
        car_attribute_identifier_idiom_value_pad, // idiom
        3,                                        // subtype
        4,                                        // graphics_class
        5,                                        // memory_class
        car_attribute_identifier_size_class_value_compact, // size_class_horizontal
        car_attribute_identifier_size_class_value_regular, // size_class_vertical
        8,                                        // identifier
        9,                                        // element
        10,                                       // part
        11,                                       // state
        12,                                       // value
        13                                        // dimension1
    };

    car::AttributeList attributes = car::AttributeList::Load(keyfmt->num_identifiers, keyfmt->identifier_list, rendition_key);

    ext::optional<uint16_t> scale = attributes.get(car_attribute_identifier_scale);
    EXPECT_TRUE(scale);
    EXPECT_TRUE(*scale == 1);

    ext::optional<uint16_t> idiom = attributes.get(car_attribute_identifier_idiom);
    EXPECT_TRUE(idiom);
    EXPECT_TRUE(*idiom == car_attribute_identifier_idiom_value_pad);

    ext::optional<uint16_t> facet_identifier = attributes.get(car_attribute_identifier_identifier);
    EXPECT_TRUE(facet_identifier);
    EXPECT_TRUE(*facet_identifier == 8);

    ext::optional<uint16_t> size_class_horizontal = attributes.get(car_attribute_identifier_size_class_horizontal);
    EXPECT_TRUE(size_class_horizontal);
    EXPECT_TRUE(*size_class_horizontal == car_attribute_identifier_size_class_value_compact);

    ext::optional<uint16_t> size_class_vertical = attributes.get(car_attribute_identifier_size_class_vertical);
    EXPECT_TRUE(size_class_vertical);
    EXPECT_TRUE(*size_class_vertical == car_attribute_identifier_size_class_value_regular);

    auto output = attributes.Write(keyfmt->num_identifiers, keyfmt->identifier_list);
    uint16_t *attributes_out = reinterpret_cast<uint16_t *>(output.data());

    EXPECT_TRUE(0 == memcmp(rendition_key, attributes_out, sizeof(uint16_t) * keyfmt->num_identifiers));
}

