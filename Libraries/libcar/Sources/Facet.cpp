/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <car/Facet.h>
#include <car/AttributeList.h>
#include <car/Rendition.h>

#include <cstring>
#include <cstdlib>
#include <map>

using car::Facet;
using car::FacetReference;
using car::AttributeList;

Facet::
Facet(std::string const &name, AttributeList const &attributes) :
    _name      (name),
    _attributes(attributes)
{
}

void Facet::
dump() const
{
    fprintf(stderr, "Facet: %s\n", _name.c_str());

    ext::optional<AttributeList> attributes = this->attributes();
    if (attributes) {
        attributes->dump();
    }
}

Facet Facet::
Create(std::string const &name, AttributeList const &attributes)
{
    return Facet(name, attributes);
}

Facet Facet::
Load(FacetReference const &reference)
{
    std::string name = std::string(reference.name(), reference.nameSize());
    struct car_facet_value const *value = static_cast<car_facet_value const *>(reference.value());
    AttributeList attributes = AttributeList::Load(value->attributes_count, value->attributes);
    return Facet(name, attributes);
}

std::vector<uint8_t> Facet::
write() const
{
    std::map<enum car_attribute_identifier, uint16_t> ordered_attributes;
    size_t attributes_count = _attributes.count();
    size_t facet_value_size = sizeof(struct car_facet_value) + (sizeof(struct car_attribute_pair) * attributes_count);
    std::vector<uint8_t> output = std::vector<uint8_t>(facet_value_size);
    struct car_facet_value *facet_value = reinterpret_cast<struct car_facet_value *>(output.data());
    facet_value->attributes_count = 0;

    _attributes.iterate([&ordered_attributes](enum car_attribute_identifier identifier, uint16_t value) {
        ordered_attributes[identifier] = value;
    });

    for (auto const &pair : ordered_attributes) {
        if (facet_value->attributes_count < attributes_count) {
            facet_value->attributes[facet_value->attributes_count].identifier = pair.first;
            facet_value->attributes[facet_value->attributes_count].value = pair.second;
            facet_value->attributes_count += 1;
        }
    }
    return output;
}

FacetReference::
FacetReference(char const *name, size_t nameSize, void const *value, size_t valueSize) :
    _name     (name),
    _nameSize (nameSize),
    _value    (value),
    _valueSize(valueSize)
{
}

AttributeList::Identifier FacetReference::
identifier() const
{
    struct car_facet_value const *value = static_cast<car_facet_value const *>(_value);
    for (size_t i = 0; i < value->attributes_count; i++) {
        if (value->attributes[i].identifier == car_attribute_identifier_identifier) {
            return value->attributes[i].value;
        }
    }

    abort();
}

