/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/car.h>
#include <car/car_format.h>

#include <stdio.h>

void
car_key_format_dump(car::Reader const *archive)
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
car_part_dump(car::Reader const *archive)
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
car_element_dump(car::Reader const *archive)
{
    struct bom_tree_context *tree = bom_tree_alloc_load(archive->bom(), car_element_info_variable);
    if (tree == NULL) {
        printf("Invalid element list.\n");
        return;
    }

    bom_tree_iterate(tree, _car_part_element_dump_iterator, NULL);
    bom_tree_free(tree);
}

