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

Reader::
Reader(unique_ptr_bom bom) :
    _bom(std::move(bom))
{
}

struct _car_iterator_ctx {
    Reader const *reader;
    void *iterator;
};

void
_car_tree_iterator(Reader const *reader, const char *tree_variable, bom_tree_iterator tree_iterator, void *iterator)
{
    assert(reader != NULL);
    assert(iterator != NULL);

    struct bom_tree_context *tree = bom_tree_alloc_load(reader->bom(), tree_variable);
    if (tree == NULL) {
        return;
    }

    struct _car_iterator_ctx iterator_ctx = {
        .reader = reader,
        .iterator = iterator,
    };
    bom_tree_iterate(tree, tree_iterator, &iterator_ctx);
    bom_tree_free(tree);
}

static void
_car_facet_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;

    car_facet_key *facet_key = (car_facet_key *)key;
    struct car_facet_value *facet_value = (struct car_facet_value *)value;

    std::string name = std::string(facet_key, key_len);
    car::AttributeList attributes = car::AttributeList::Load(facet_value->attributes_count, facet_value->attributes);
    car::Facet facet = car::Facet::Create(name, attributes);

    (*reinterpret_cast<std::function<void(car::Facet const &)> const *>(iterator_ctx->iterator))(facet);
}

void Reader::
facetIterate(std::function<void(Facet const &)> const &iterator) const
{
    _car_tree_iterator(this, car_facet_keys_variable, _car_facet_iterator, const_cast<void *>(reinterpret_cast<void const *>(&iterator)));
}

static void
_car_rendition_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;

    car_rendition_key *rendition_key = (car_rendition_key *)key;
    struct car_rendition_value *rendition_value = (struct car_rendition_value *)value;

    int key_format_index = bom_variable_get(iterator_ctx->reader->bom(), car_key_format_variable);
    struct car_key_format *keyfmt = (struct car_key_format *)bom_index_get(iterator_ctx->reader->bom(), key_format_index, NULL);

    car::AttributeList attributes = car::AttributeList::Load(keyfmt->num_identifiers, keyfmt->identifier_list, rendition_key);
    car::Rendition rendition = car::Rendition::Load(attributes, rendition_value);

    (*reinterpret_cast<std::function<void(car::Rendition const &)> const *>(iterator_ctx->iterator))(rendition);
}

void Reader::
renditionIterate(std::function<void(Rendition const &)> const &iterator) const
{
    _car_tree_iterator(this, car_renditions_variable, _car_rendition_iterator, const_cast<void *>(reinterpret_cast<void const *>(&iterator)));
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

ext::optional<Reader> Reader::
Load(unique_ptr_bom bom)
{
    int header_index = bom_variable_get(bom.get(), car_header_variable);

    size_t header_len;
    struct car_header *header = (struct car_header *)bom_index_get(bom.get(), header_index, &header_len);
    if (header_len < sizeof(struct car_header) || strncmp(header->magic, "RATC", 4) || header->storage_version < 8) {
        return ext::nullopt;
    }

    return Reader(std::move(bom));
}

ext::optional<Reader> Reader::
Create(unique_ptr_bom bom)
{
    struct car_header *header = (struct car_header *)malloc(sizeof(struct car_header));
    if (header == NULL) {
        return ext::nullopt;
    }

    strncpy(header->magic, "RATC", 4);
    header->ui_version = 0x131; // todo
    header->storage_version = 0xC; // todo
    header->storage_timestamp = time(NULL); // todo
    header->rendition_count = 0;
    strncpy(header->file_creator, "asset catalog compiler\n", sizeof(header->file_creator));
    strncpy(header->other_creator, "version 1.0", sizeof(header->other_creator));

    std::random_device device;
    std::uniform_int_distribution<int> distribution = std::uniform_int_distribution<int>(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
    for (size_t i = 0; i < sizeof(header->uuid); i++) {
        header->uuid[i] = distribution(device);
    }

    header->associated_checksum = 0; // todo
    header->schema_version = 4; // todo
    header->color_space_id = 1; // todo
    header->key_semantics = 1; // todo

    int header_index = bom_index_add(bom.get(), header, sizeof(struct car_header));
    bom_variable_add(bom.get(), car_header_variable, header_index);
    free(header);

    struct car_key_format *keyfmt = (struct car_key_format *)malloc(sizeof(struct car_key_format));
    if (keyfmt == NULL) {
        return ext::nullopt;
    }

    strncpy(keyfmt->magic, "tmfk", 4);
    keyfmt->reserved = 0;
    keyfmt->num_identifiers = 0;

    int key_format_index = bom_index_add(bom.get(), keyfmt, sizeof(struct car_key_format));
    bom_variable_add(bom.get(), car_key_format_variable, key_format_index);
    free(keyfmt);

    struct bom_tree_context *facet_tree = bom_tree_alloc_empty(bom.get(), car_facet_keys_variable);
    bom_tree_free(facet_tree);

    struct bom_tree_context *rendition_tree = bom_tree_alloc_empty(bom.get(), car_renditions_variable);
    bom_tree_free(rendition_tree);

    return Reader(std::move(bom));
}

