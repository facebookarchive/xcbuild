/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/car.h>
#include <car/car_format.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

const char *const car_header_variable = "CARHEADER";
const char *const car_key_format_variable = "KEYFORMAT";
const char *const car_facet_keys_variable = "FACETKEYS";
const char *const car_part_info_variable = "PART_INFO";
const char *const car_element_info_variable = "ELEMENT_INFO";
const char *const car_renditions_variable = "RENDITIONS";
const char *const car_colors_variable = "COLORS";
const char *const car_fonts_variable = "FONTS";
const char *const car_font_sizes_variable = "FONTSIZES";
const char *const car_glyphs_variable = "GLYPHS";
const char *const car_bezels_variable = "BEZELS";

const char *const car_attribute_identifier_names[_car_attribute_identifier_count] = {
    [car_attribute_identifier_element] = "element",
    [car_attribute_identifier_part] = "part",
    [car_attribute_identifier_size] = "size",
    [car_attribute_identifier_direction] = "direction",
    [car_attribute_identifier_value] = "value",
    [car_attribute_identifier_dimension1] = "dimension1",
    [car_attribute_identifier_dimension2] = "dimension2",
    [car_attribute_identifier_state] = "state",
    [car_attribute_identifier_layer] = "layer",
    [car_attribute_identifier_scale] = "scale",
    [car_attribute_identifier_presentation_state] = "presentation_state",
    [car_attribute_identifier_idiom] = "idiom",
    [car_attribute_identifier_subtype] = "subtype",
    [car_attribute_identifier_identifier] = "identifier",
    [car_attribute_identifier_previous_value] = "previous_value",
    [car_attribute_identifier_previous_state] = "previous_state",
    [car_attribute_identifier_size_class_horizontal] = "size_class_horizontal",
    [car_attribute_identifier_size_class_vertical] = "size_class_vertical",
    [car_attribute_identifier_memory_class] = "memory_class",
    [car_attribute_identifier_graphics_class] = "graphics_class",
};


/* Context */

struct car_context {
    struct bom_context *bom;
};

static struct car_context *
_car_alloc(struct bom_context *bom)
{
    assert(bom != NULL);

    struct car_context *context = (struct car_context *)malloc(sizeof(*context));
    if (context == NULL) {
        bom_free(bom);
        return NULL;
    }

    context->bom = bom;

    return context;
}

struct car_context *
car_alloc_load(struct bom_context *bom)
{
    struct car_context *context = _car_alloc(bom);
    if (context == NULL) {
        return NULL;
    }

    int header_index = bom_variable_get(context->bom, car_header_variable);
    size_t header_len;
    struct car_header *header = (struct car_header *)bom_index_get(context->bom, header_index, &header_len);
    if (header_len < sizeof(struct car_header) || strncmp(header->magic, "RATC", 4) || header->storage_version < 8) {
        car_free(context);
        return NULL;
    }

    return context;
}

struct car_context *
car_alloc_empty(struct bom_context *bom)
{
    struct car_context *context = _car_alloc(bom);
    if (context == NULL) {
        return NULL;
    }

    struct car_header *header = (struct car_header *)malloc(sizeof(struct car_header));
    if (header == NULL) {
        car_free(context);
        return NULL;
    }

    strncpy(header->magic, "RATC", 4);
    header->ui_version = 0x131; // todo
    header->storage_version = 8;
    header->storage_timestamp = time(NULL); // todo
    header->rendition_count = 0;
    strncpy(header->file_creator, "asset catalog compiler\n", sizeof(header->file_creator));
    strncpy(header->other_creator, "version 1.0", sizeof(header->other_creator));
    arc4random_buf(&header->uuid, sizeof(header->uuid));
    header->associated_checksum = 0; // todo
    header->schema_version = 4; // todo
    header->color_space_id = 1; // todo
    header->key_semantics = 1; // todo

    int header_index = bom_index_add(context->bom, header, sizeof(struct car_header));
    bom_variable_add(context->bom, car_header_variable, header_index);
    free(header);

    struct car_key_format *keyfmt = (struct car_key_format *)malloc(sizeof(struct car_key_format));
    if (keyfmt == NULL) {
        car_free(context);
        return NULL;
    }

    strncpy(keyfmt->magic, "tmfk", 4);
    keyfmt->reserved = 0;
    keyfmt->num_identifiers = 0;

    int key_format_index = bom_index_add(context->bom, keyfmt, sizeof(struct car_key_format));
    bom_variable_add(context->bom, car_key_format_variable, key_format_index);
    free(keyfmt);

    struct bom_tree_context *facet_tree = bom_tree_alloc_empty(context->bom, car_facet_keys_variable);
    bom_tree_free(facet_tree);

    struct bom_tree_context *rendition_tree = bom_tree_alloc_empty(context->bom, car_renditions_variable);
    bom_tree_free(rendition_tree);

    struct bom_tree_context *part_info_tree = bom_tree_alloc_empty(context->bom, car_part_info_variable);
    bom_tree_free(part_info_tree);

    struct bom_tree_context *element_info_tree = bom_tree_alloc_empty(context->bom, car_element_info_variable);
    bom_tree_free(element_info_tree);

    struct bom_tree_context *colors_tree = bom_tree_alloc_empty(context->bom, car_colors_variable);
    bom_tree_free(colors_tree);

    struct bom_tree_context *fonts_tree = bom_tree_alloc_empty(context->bom, car_fonts_variable);
    bom_tree_free(fonts_tree);

    struct bom_tree_context *font_sizes_tree = bom_tree_alloc_empty(context->bom, car_font_sizes_variable);
    bom_tree_free(font_sizes_tree);

    struct bom_tree_context *glyphs_tree = bom_tree_alloc_empty(context->bom, car_glyphs_variable);
    bom_tree_free(glyphs_tree);

    struct bom_tree_context *bezels_tree = bom_tree_alloc_empty(context->bom, car_bezels_variable);
    bom_tree_free(bezels_tree);

    return context;
}

void
car_free(struct car_context *context)
{
    if (context == NULL) {
        return;
    }

    bom_free(context->bom);
    free(context);
}

struct bom_context *
car_bom_get(struct car_context *context)
{
    return context->bom;
}


/* Tree Iterator */

struct _car_iterator_ctx {
    struct car_context *context;
    void *ctx;
    void *iterator;
};

void
_car_tree_iterator(struct car_context *context, const char *tree_variable, bom_tree_iterator tree_iterator, void *iterator, void *ctx)
{
    assert(context != NULL);
    assert(iterator != NULL);

    struct bom_tree_context *tree = bom_tree_alloc_load(context->bom, tree_variable);
    if (tree == NULL) {
        return;
    }

    struct _car_iterator_ctx iterator_ctx = {
        .context = context,
        .ctx = ctx,
        .iterator = iterator,
    };
    bom_tree_iterate(tree, tree_iterator, &iterator_ctx);
    bom_tree_free(tree);
}


/* Facet */

static void
_car_facet_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    car_facet_key *facet_key = (car_facet_key *)key;

    char *name = (char *)malloc(key_len + 1);
    if (name == NULL) {
        return;
    }
    strncpy(name, facet_key, key_len);
    name[key_len] = '\0';

    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;
    ((car_facet_iterator)iterator_ctx->iterator)(iterator_ctx->context, name, iterator_ctx->ctx);

    free(name);
}

void
car_facet_iterate(struct car_context *context, car_facet_iterator iterator, void *ctx)
{
    _car_tree_iterator(context, car_facet_keys_variable, _car_facet_iterator, (void *)iterator, ctx);
}


/* Rendition */

static void
_car_rendition_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    car_rendition_key *rendition_key = (car_rendition_key *)key;
    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;

    int key_format_index = bom_variable_get(iterator_ctx->context->bom, car_key_format_variable);
    struct car_key_format *keyfmt = (struct car_key_format *)bom_index_get(iterator_ctx->context->bom, key_format_index, NULL);

    car::AttributeList attributes = car::AttributeList::Load(keyfmt->num_identifiers, (enum car_attribute_identifier *)keyfmt->identifier_list, rendition_key);
    ((car_rendition_iterator)iterator_ctx->iterator)(iterator_ctx->context, attributes, iterator_ctx->ctx);
}

void
car_rendition_iterate(struct car_context *context, car_rendition_iterator iterator, void *ctx)
{
    _car_tree_iterator(context, car_renditions_variable, _car_rendition_iterator, (void *)iterator, ctx);
}


/* Debugging */

void
car_header_dump(struct car_context *context)
{
    int header_index = bom_variable_get(context->bom, car_header_variable);
    struct car_header *header = (struct car_header *)bom_index_get(context->bom, header_index, NULL);

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
car_key_format_dump(struct car_context *context)
{
    int key_format_index = bom_variable_get(context->bom, car_key_format_variable);
    struct car_key_format *keyfmt = (struct car_key_format *)bom_index_get(context->bom, key_format_index, NULL);

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
car_part_dump(struct car_context *context)
{
    struct bom_tree_context *tree = bom_tree_alloc_load(context->bom, car_part_info_variable);
    if (tree == NULL) {
        printf("Invalid part list.\n");
        return;
    }

    bom_tree_iterate(tree, _car_part_element_dump_iterator, NULL);
    bom_tree_free(tree);
}

void
car_element_dump(struct car_context *context)
{
    struct bom_tree_context *tree = bom_tree_alloc_load(context->bom, car_element_info_variable);
    if (tree == NULL) {
        printf("Invalid element list.\n");
        return;
    }

    bom_tree_iterate(tree, _car_part_element_dump_iterator, NULL);
    bom_tree_free(tree);
}

