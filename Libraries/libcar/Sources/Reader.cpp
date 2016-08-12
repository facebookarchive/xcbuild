/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <car/Reader.h>
#include <car/Facet.h>
#include <car/Rendition.h>
#include <car/car_format.h>

#include <limits>
#include <random>

#include <cassert>
#include <cstring>

using car::Reader;
using car::AttributeList;
using car::Facet;
using car::FacetReference;
using car::Rendition;
using car::RenditionReference;

Reader::
Reader(unique_ptr_bom bom) :
    _bom(std::move(bom))
{
}

void Reader::
iterateFacets(std::function<void(std::string const &, FacetReference const &)> const &iterator) const
{
    for (auto const &entry : _facetReferences) {
        iterator(entry.first, entry.second);
    }
}

void Reader::
iterateRenditions(ext::optional<AttributeList::Identifier> const &identifier, std::function<void(AttributeList::Identifier const &, RenditionReference const &)> const &iterator) const
{
    auto range = std::make_pair(_renditionReferences.begin(), _renditionReferences.end());
    if (identifier) {
        /* If specified, limit to just matching renditions. */
        range = _renditionReferences.equal_range(*identifier);
    }

    for (auto it = range.first; it != range.second; ++it) {
        iterator(it->first, it->second);
    }
}

ext::optional<FacetReference>
Reader::
facet(std::string const &name) const
{
    auto it = _facetReferences.find(name);
    if (it == _facetReferences.end()) {
        return ext::nullopt;
    }

    return it->second;
}

std::vector<RenditionReference> Reader::
renditions(AttributeList::Identifier const &identifier) const
{
    std::vector<RenditionReference> result;

    auto range = _renditionReferences.equal_range(identifier);
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second);
    }

    return result;
}

void Reader::
dump() const
{
    int header_index = bom_variable_get(_bom.get(), car_header_variable);
    struct car_header *header = (struct car_header *)bom_index_get(_bom.get(), header_index, NULL);

    printf("Magic: %.4s\n", header->magic);
    printf("UI version: %x\n", header->ui_version);
    printf("Storage version: %x\n", header->storage_version);
    printf("Storage Timestamp: %x\n", header->storage_timestamp);
    printf("Rendition Count: %x\n", header->rendition_count);
    printf("Creator: %s\n", header->file_creator);
    printf("Other Creator: %s\n", header->other_creator);
    printf("UUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n", header->uuid[0], header->uuid[1], header->uuid[2], header->uuid[3], header->uuid[4], header->uuid[5], header->uuid[6], header->uuid[7], header->uuid[8], header->uuid[9], header->uuid[10], header->uuid[11], header->uuid[12], header->uuid[13], header->uuid[14], header->uuid[15]);
    printf("Associated Checksum: %x\n", header->associated_checksum);
    printf("Schema Version: %x\n", header->schema_version);
    printf("Color space ID: %x\n", header->color_space_id);
    printf("Key Semantics: %x\n", header->key_semantics);
    printf("\n");

    int key_format_index = bom_variable_get(_bom.get(), car_key_format_variable);
    struct car_key_format *keyfmt = (struct car_key_format *)bom_index_get(_bom.get(), key_format_index, NULL);

    printf("Key Format: %.4s\n", keyfmt->magic);
    printf("Identifier Count: %d\n", keyfmt->num_identifiers);
    for (uint32_t i = 0; i < keyfmt->num_identifiers; i++) {
        uint32_t identifier = keyfmt->identifier_list[i];
        if (identifier < sizeof(car_attribute_identifier_names) / sizeof(*car_attribute_identifier_names)) {
            printf("Identifier: %s (%d)\n", car_attribute_identifier_names[identifier] ?: "(unknown)", identifier);
        } else {
            printf("Identifier: (unknown) (%d)\n", identifier);
        }
    }
    printf("\n");

    auto part_element_iterator = [](struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx) {
        struct car_part_element_key *part_element_key = (struct car_part_element_key *)key;
        printf("%s ID: %x\n", (char const *)ctx, part_element_key->part_element_id);

        struct car_part_element_value *part_element_value = (struct car_part_element_value *)value;
        printf("Unknown 1: %x\n", part_element_value->unknown1);
        printf("Unknown 2: %x\n", part_element_value->unknown2);
        printf("Name: %.*s\n", (int)(value_len - sizeof(struct car_part_element_value)), part_element_value->name);
        printf("\n");
    };

    struct bom_tree_context *part = bom_tree_alloc_load(_bom.get(), car_part_info_variable);
    if (part != NULL) {
        bom_tree_iterate(part, part_element_iterator, (void *)"Part");
        bom_tree_free(part);
    }

    struct bom_tree_context *element = bom_tree_alloc_load(_bom.get(), car_element_info_variable);
    if (element != NULL) {
        bom_tree_iterate(element, part_element_iterator, (void *)"Element");
        bom_tree_free(element);
    }
}

template<typename T>
static void
_car_tree_iterator_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    T const *iterator = reinterpret_cast<T const *>(ctx);
    (*iterator)(tree, key, key_len, value, value_len);
}

template<typename T>
static void
_car_tree_iterator(struct bom_context *bom, const char *tree_variable, T const &iterator)
{
    assert(bom != NULL);
    struct bom_tree_context *tree = bom_tree_alloc_load(bom, tree_variable);
    if (tree == NULL) {
        return;
    }

    bom_tree_iterate(tree, _car_tree_iterator_iterator<T>, const_cast<void *>(reinterpret_cast<void const *>(&iterator)));
    bom_tree_free(tree);
}

ext::optional<Reader> Reader::
Load(unique_ptr_bom bom)
{
    /*
     * Load the archive header.
     */
    int header_index = bom_variable_get(bom.get(), car_header_variable);

    size_t header_len;
    struct car_header *header = (struct car_header *)bom_index_get(bom.get(), header_index, &header_len);
    if (header == NULL || header_len < sizeof(struct car_header) || strncmp(header->magic, "RATC", 4) || header->storage_version < 8) {
        return ext::nullopt;
    }

    /*
     * Load the rendition key format.
     */
    int key_format_index = bom_variable_get(bom.get(), car_key_format_variable);
    struct car_key_format const *keyfmt = (struct car_key_format *)bom_index_get(bom.get(), key_format_index, NULL);
    if (keyfmt == NULL) {
        return ext::nullopt;
    }

    /*
     * Find the rendition attribute list index for the facet identifier.
     */
    ext::optional<size_t> identifierIndex = 0;
    for (size_t i = 0; i < keyfmt->num_identifiers; i++) {
        if (keyfmt->identifier_list[i] == car_attribute_identifier_identifier) {
            identifierIndex = i;
            break;
        }
    }
    if (!identifierIndex) {
        return ext::nullopt;
    }

    auto reader = Reader(std::move(bom));

    /*
     * Create index of facets by name.
     */
    _car_tree_iterator(reader.bom(), car_facet_keys_variable, [&reader](struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len) {
        FacetReference reference = FacetReference(static_cast<char const *>(key), key_len, value, value_len);
        std::string name = std::string(reference.name(), reference.nameSize());
        reader._facetReferences.insert({ name, reference });
    });

    /*
     * Create index of renditions by facet.
     */
    _car_tree_iterator(reader.bom(), car_renditions_variable, [&reader, keyfmt, identifierIndex](struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len) {
        RenditionReference reference = RenditionReference(keyfmt->num_identifiers, keyfmt->identifier_list, *identifierIndex, static_cast<uint16_t const *>(key), value, value_len);
        reader._renditionReferences.insert({ reference.identifier(), reference });
    });

    return std::move(reader);
}
