/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <car/AttributeList.h>

using car::AttributeList;

static enum car_attribute_identifier const IdentifierAttribute = car_attribute_identifier_identifier;

AttributeList::
AttributeList(Identifier const &identifier, std::unordered_map<enum car_attribute_identifier, uint16_t> const &values) :
    _values(values)
{
    _values[IdentifierAttribute] = identifier;
}

bool AttributeList::
operator==(AttributeList const &rhs) const
{
    return _values == rhs._values;
}

bool AttributeList::
operator!=(AttributeList const &rhs) const
{
    return !(*this == rhs);
}

AttributeList::Identifier AttributeList::
identifier() const
{
    return *get(IdentifierAttribute);
}

ext::optional<uint16_t> AttributeList::
get(enum car_attribute_identifier identifier) const
{
    auto it = _values.find(identifier);
    if (it != _values.end()) {
        return it->second;
    }

    return ext::nullopt;
}

void AttributeList::
set(enum car_attribute_identifier identifier, uint16_t value)
{
    _values[identifier] = value;
}

size_t AttributeList::
count() const
{
    return _values.size();
}

void AttributeList::
dump() const
{
    for (auto const &entry : _values) {
        enum car_attribute_identifier identifier = entry.first;
        uint16_t value = entry.second;

        if (identifier < sizeof(car_attribute_identifier_names) / sizeof(*car_attribute_identifier_names)) {
            printf("[%02d] %-24s = %-6d | %-4x\n", identifier, car_attribute_identifier_names[identifier] ?: "(unknown)", value, value);
        } else {
            printf("[%02d] %-24s = %-6d | %-4x\n", identifier, "(unknown)", value, value);
        }
    }
}

AttributeList AttributeList::
Load(size_t count, uint32_t const *identifiers, uint16_t const *values)
{
    ext::optional<Identifier> identifier;

    std::unordered_map<enum car_attribute_identifier, uint16_t> attributes;
    for (size_t i = 0; i < count; ++i) {
        uint16_t value = values[i];
        enum car_attribute_identifier attribute = static_cast<enum car_attribute_identifier>(identifiers[i]);

        if (attribute == IdentifierAttribute) {
            identifier = value;
        } else {
            attributes.insert({ attribute, value });
        }
    }

    if (!identifier) {
        abort();
    }

    return AttributeList(*identifier, attributes);
}

AttributeList AttributeList::
Load(size_t count, struct car_attribute_pair const *pairs)
{
    ext::optional<Identifier> identifier;

    std::unordered_map<enum car_attribute_identifier, uint16_t> attributes;
    for (size_t i = 0; i < count; ++i) {
        uint16_t value = pairs[i].value;
        enum car_attribute_identifier attribute = static_cast<enum car_attribute_identifier>(pairs[i].identifier);

        if (attribute == IdentifierAttribute) {
            identifier = value;
        } else {
            attributes.insert({ attribute, value });
        }
    }

    if (!identifier) {
        abort();
    }

    return AttributeList(*identifier, attributes);
}

size_t std::hash<AttributeList>::
operator()(AttributeList const &attributes) const
{
    /* Not a great hash. */
    return attributes.count();
}

std::vector<uint8_t> AttributeList::
write(size_t count, uint32_t const *identifiers) const
{
    std::vector<uint8_t> output = std::vector<uint8_t>(sizeof(uint16_t) * count);
    uint16_t *values = reinterpret_cast<uint16_t *>(output.data());
    std::unordered_map<enum car_attribute_identifier, uint16_t> attributes;
    for (size_t i = 0; i < count; ++i) {
        enum car_attribute_identifier identifier = (enum car_attribute_identifier) identifiers[i];
        auto result = _values.find(identifier);
        if (result != _values.end()) {
            values[i] = result->second;
        } else {
            values[i] = 0;
        }
    }
    return output;
}

