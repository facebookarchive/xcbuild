/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/car.h>
#include <car/car_format.h>

#include <stdio.h>

void
car_header_dump(car::Archive const *archive)
{
    int header_index = bom_variable_get(archive->bom(), car_header_variable);
    struct car_header *header = (struct car_header *)bom_index_get(archive->bom(), header_index, NULL);

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
}

void
car_key_format_dump(car::Archive const *archive)
{
    int key_format_index = bom_variable_get(archive->bom(), car_key_format_variable);
    struct car_key_format *keyfmt = (struct car_key_format *)bom_index_get(archive->bom(), key_format_index, NULL);

    printf("Magic: %.4s\n", keyfmt->magic);
    printf("Identifier Count: %d\n", keyfmt->num_identifiers);
    for (uint32_t i = 0; i < keyfmt->num_identifiers; i++) {
        uint32_t identifier = keyfmt->identifier_list[i];
        if (identifier < sizeof(car_attribute_identifier_names) / sizeof(*car_attribute_identifier_names)) {
            printf("Identifier: %s (%d)\n", car_attribute_identifier_names[identifier] ?: "(unknown)", identifier);
        } else {
            printf("Identifier: (unknown) (%d)\n", identifier);
        }
    }
}

static void
_car_part_element_dump_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    struct car_part_element_key *part_element_key = (struct car_part_element_key *)key;
    printf("ID: %x\n", part_element_key->part_element_id);

    struct car_part_element_value *part_element_value = (struct car_part_element_value *)value;
    printf("Unknown 1: %x\n", part_element_value->unknown1);
    printf("Unknown 2: %x\n", part_element_value->unknown2);
    printf("Name: %.*s\n", (int)(value_len - sizeof(struct car_part_element_value)), part_element_value->name);
}

void
car_part_dump(car::Archive const *archive)
{
    struct bom_tree_context *tree = bom_tree_alloc_load(archive->bom(), car_part_info_variable);
    if (tree == NULL) {
        printf("Invalid part list.\n");
        return;
    }

    bom_tree_iterate(tree, _car_part_element_dump_iterator, NULL);
    bom_tree_free(tree);
}

void
car_element_dump(car::Archive const *archive)
{
    struct bom_tree_context *tree = bom_tree_alloc_load(archive->bom(), car_element_info_variable);
    if (tree == NULL) {
        printf("Invalid element list.\n");
        return;
    }

    bom_tree_iterate(tree, _car_part_element_dump_iterator, NULL);
    bom_tree_free(tree);
}

