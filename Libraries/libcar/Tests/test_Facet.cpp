/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <car/Facet.h>
#include <car/AttributeList.h>
#include <car/car_format.h>

struct test_car_facet_value {
    struct car_facet_value header;
    struct car_attribute_pair attributes[3];
} __attribute__((packed));

static struct test_car_facet_value test_facet_value_s = {
    {
        { 0, 0 },
        3,
    },
    {
        { car_attribute_identifier_element, 85 },
        { car_attribute_identifier_part, 181 },
        { car_attribute_identifier_identifier, 4258 },
    }
};

static struct car_facet_value *test_facet_value = &(test_facet_value_s.header);

TEST(Facet, Deserialize)
{
    auto facet = car::Facet::Load("name", test_facet_value);
    EXPECT_EQ(facet.name(), "name");
    EXPECT_EQ(*facet.attributes().get(car_attribute_identifier_element), 85);
    EXPECT_EQ(*facet.attributes().get(car_attribute_identifier_part), 181);
    EXPECT_EQ(*facet.attributes().get(car_attribute_identifier_identifier), 4258);
}

TEST(Facet, Serialize)
{
    std::vector<uint8_t> test_facet_value_vector(reinterpret_cast<uint8_t *>(test_facet_value),
        reinterpret_cast<uint8_t *>(test_facet_value) + sizeof(test_facet_value_s));

    car::AttributeList attributes = car::AttributeList({
        { car_attribute_identifier_element, 85 },
        { car_attribute_identifier_part, 181 },
        { car_attribute_identifier_identifier, 4258 },
    });

    car::Facet facet = car::Facet::Create("name", attributes);
    auto facet_value = facet.write();

    EXPECT_EQ(facet_value, test_facet_value_vector);
}

