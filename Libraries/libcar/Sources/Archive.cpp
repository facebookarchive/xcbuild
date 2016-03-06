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
    std::string name = std::string(facet_key, key_len);

    struct _car_iterator_ctx *iterator_ctx = (struct _car_iterator_ctx *)ctx;
    ((car::Archive::FacetIterator)iterator_ctx->iterator)(iterator_ctx->archive, name, iterator_ctx->ctx);
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

void Archive::
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
    header->storage_version = 0xC; // todo
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

    return Archive(std::move(bom));
}

