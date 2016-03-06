/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/Archive.h>
#include <car/car_format.h>

#include <cassert>

using car::Archive;

Archive::
Archive(unique_ptr_bom bom) :
    _bom(std::move(bom))
{
}

struct _car_iterator_ctx {
    Archive const *archive;
    void *ctx;
    void *iterator;
};

void
_car_tree_iterator(Archive const *archive, const char *tree_variable, bom_tree_iterator tree_iterator, void *iterator, void *ctx)
{
    assert(archive != NULL);
    assert(iterator != NULL);

    struct bom_tree_context *tree = bom_tree_alloc_load(archive->bom(), tree_variable);
    if (tree == NULL) {
        return;
    }

    struct _car_iterator_ctx iterator_ctx = {
        .archive = archive,
        .ctx = ctx,
        .iterator = iterator,
    };
    bom_tree_iterate(tree, tree_iterator, &iterator_ctx);
    bom_tree_free(tree);
}

static void
_car_facet_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    car_facet_key *facet_key = (car_facet_key *)key;
    struct car_facet_value *facet_value = (struct car_facet_value *)value;

    car::Facet facet = car::Facet(
        std::string(facet_key, key_len),
        car::AttributeList::Load(facet_value->attributes_count, facet_value->attributes));

    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;
    ((car::Archive::FacetIterator)iterator_ctx->iterator)(iterator_ctx->archive, facet, iterator_ctx->ctx);
}

void Archive::
facetIterate(FacetIterator iterator, void *ctx) const
{
    _car_tree_iterator(this, car_facet_keys_variable, _car_facet_iterator, (void *)iterator, ctx);
}

static void
_car_rendition_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    car_rendition_key *rendition_key = (car_rendition_key *)key;
    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;

    int key_format_index = bom_variable_get(iterator_ctx->archive->bom(), car_key_format_variable);
    struct car_key_format *keyfmt = (struct car_key_format *)bom_index_get(iterator_ctx->archive->bom(), key_format_index, NULL);

    car::AttributeList attributes = car::AttributeList::Load(keyfmt->num_identifiers, keyfmt->identifier_list, rendition_key);
    ((car::Archive::RenditionIterator)iterator_ctx->iterator)(iterator_ctx->archive, attributes, iterator_ctx->ctx);
}

void Archive::
renditionIterate(RenditionIterator iterator, void *ctx) const
{
    _car_tree_iterator(this, car_renditions_variable, _car_rendition_iterator, (void *)iterator, ctx);
}

struct _car_facet_rendition_iterate_ctx {
    car::Facet const *facet;
    uint16_t facet_identifier;
    car::Archive::FacetRenditionIterator iterator;
    void *ctx;
};

static void
_car_facet_rendition_iterator(Archive const *archive, car::AttributeList const &attributes, void *ctx)
{
    struct _car_facet_rendition_iterate_ctx *iterate_ctx = (struct _car_facet_rendition_iterate_ctx *)ctx;

    ext::optional<uint16_t> rendition_identifier = attributes.get(car_attribute_identifier_identifier);
    if (rendition_identifier && *rendition_identifier == iterate_ctx->facet_identifier) {
        ((car::Archive::FacetRenditionIterator)iterate_ctx->iterator)(archive, iterate_ctx->facet, attributes, iterate_ctx->ctx);
    }
}

void Archive::
facetRenditionIterate(Facet const &facet, FacetRenditionIterator iterator, void *ctx) const
{
    ext::optional<uint16_t> facet_identifier = facet.attributes().get(car_attribute_identifier_identifier);
    if (!facet_identifier) {
        return;
    }

    struct _car_facet_rendition_iterate_ctx iterate_ctx = {
        .facet = &facet,
        .facet_identifier = *facet_identifier,
        .iterator = iterator,
        .ctx = ctx,
    };
    renditionIterate(_car_facet_rendition_iterator, &iterate_ctx);
}

struct _car_facet_exists_ctx {
    std::string const *name;
    bool exists;
};

static void
_car_facet_exists_iterator(Archive const *context, car::Facet const &facet, void *ctx)
{
    struct _car_facet_exists_ctx *exists_ctx = (struct _car_facet_exists_ctx *)ctx;
    exists_ctx->exists = exists_ctx->exists || facet.name() == *exists_ctx->name;
}

bool Archive::
facetAdd(Facet const &facet)
{
    /* Verify facet does not yet exist. */
    struct _car_facet_exists_ctx ctx = { .name = &facet.name(), .exists = false };
    facetIterate(_car_facet_exists_iterator, &ctx);
    if (ctx.exists) {
        return false;
    }

    size_t key_len = sizeof(car_facet_key) + facet.name().size();
    car_facet_key *key = (car_facet_key *)malloc(key_len);
    if (key == NULL) {
        return false;
    }
    strncpy(key, facet.name().c_str(), facet.name().size());

    size_t attribute_count = facet.attributes().count();
    size_t attribute_len = sizeof(struct car_facet_value) + sizeof(struct car_attribute_pair) * attribute_count;
    struct car_facet_value *value = (struct car_facet_value *)malloc(attribute_len);
    if (value == NULL) {
        free(key);
        return false;
    }
    value->hot_spot.x = 0; // todo
    value->hot_spot.y = 0; // todo
    value->attributes_count = attribute_count;

    size_t index = 0;
    facet.attributes().iterate([&](enum car_attribute_identifier identifier, uint16_t attribute_value) {
        struct car_attribute_pair *attribute = &value->attributes[index];
        attribute->identifier = identifier;
        attribute->value = attribute_value;
    });

    struct bom_tree_context *tree = bom_tree_alloc_load(_bom.get(), car_facet_keys_variable);
    if (tree == NULL) {
        free(key);
        free(value);
        return false;
    }

    bom_tree_add(tree, key, key_len, value, attribute_len);

    free(key);
    free(value);
    bom_tree_free(tree);

    return true;
}

ext::optional<Archive> Archive::
Load(unique_ptr_bom bom)
{
    int header_index = bom_variable_get(bom.get(), car_header_variable);

    size_t header_len;
    struct car_header *header = (struct car_header *)bom_index_get(bom.get(), header_index, &header_len);
    if (header_len < sizeof(struct car_header) || strncmp(header->magic, "RATC", 4) || header->storage_version < 8) {
        return ext::nullopt;
    }

    return Archive(std::move(bom));
}

ext::optional<Archive> Archive::
Create(unique_ptr_bom bom)
{
    struct car_header *header = (struct car_header *)malloc(sizeof(struct car_header));
    if (header == NULL) {
        return ext::nullopt;
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

    struct bom_tree_context *part_info_tree = bom_tree_alloc_empty(bom.get(), car_part_info_variable);
    bom_tree_free(part_info_tree);

    struct bom_tree_context *element_info_tree = bom_tree_alloc_empty(bom.get(), car_element_info_variable);
    bom_tree_free(element_info_tree);

    struct bom_tree_context *colors_tree = bom_tree_alloc_empty(bom.get(), car_colors_variable);
    bom_tree_free(colors_tree);

    struct bom_tree_context *fonts_tree = bom_tree_alloc_empty(bom.get(), car_fonts_variable);
    bom_tree_free(fonts_tree);

    struct bom_tree_context *font_sizes_tree = bom_tree_alloc_empty(bom.get(), car_font_sizes_variable);
    bom_tree_free(font_sizes_tree);

    struct bom_tree_context *glyphs_tree = bom_tree_alloc_empty(bom.get(), car_glyphs_variable);
    bom_tree_free(glyphs_tree);

    struct bom_tree_context *bezels_tree = bom_tree_alloc_empty(bom.get(), car_bezels_variable);
    bom_tree_free(bezels_tree);

    return Archive(std::move(bom));
}

