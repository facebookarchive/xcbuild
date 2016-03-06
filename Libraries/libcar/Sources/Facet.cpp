/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/Facet.h>
#include <car/Archive.h>

#include <cstring>
#include <cstdlib>

using car::Facet;

Facet::
Facet(Archive const *archive, std::string const &name) :
    _archive(archive),
    _name   (name)
{
}

typedef void (*_car_facet_value_callback)(Facet const *facet, struct car_facet_value *value, void *ctx);

struct _car_facet_value_ctx {
    Facet const *facet;
    _car_facet_value_callback callback;
    void *ctx;
};

static void
_car_facet_value_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    car_facet_key *facet_key = (car_facet_key *)key;
    struct car_facet_value *facet_value = (struct car_facet_value *)value;

    struct _car_facet_value_ctx *value_ctx = (struct _car_facet_value_ctx *)ctx;

    std::string name = std::string(facet_key, key_len);
    if (name == value_ctx->facet->name()) {
        value_ctx->callback(value_ctx->facet, facet_value, value_ctx->ctx);
    }
}

static void
_car_facet_value_get(Facet const *facet, _car_facet_value_callback callback, void *ctx)
{
    struct bom_tree_context *tree = bom_tree_alloc_load(facet->archive()->bom(), car_facet_keys_variable);
    if (tree == NULL) {
        return;
    }

    struct _car_facet_value_ctx value_ctx = {
        .facet = facet,
        .callback = callback,
        .ctx = ctx,
    };
    bom_tree_iterate(tree, _car_facet_value_iterator, &value_ctx);
    bom_tree_free(tree);
}

struct _car_facet_attributes_ctx {
    ext::optional<car::AttributeList> attributes;
};

static void
_car_facet_attributes_callback(Facet const *facet, struct car_facet_value *value, void *ctx)
{
    struct _car_facet_attributes_ctx *attributes_ctx = (struct _car_facet_attributes_ctx *)ctx;
    attributes_ctx->attributes = car::AttributeList::Load(value->attributes_count, value->attributes);
}

ext::optional<car::AttributeList> Facet::
attributes() const
{
    struct _car_facet_attributes_ctx attributes_ctx = {
        .attributes = ext::nullopt,
    };
    _car_facet_value_get(this, _car_facet_attributes_callback, &attributes_ctx);
    return attributes_ctx.attributes;
}

void Facet::
dump() const
{
    fprintf(stderr, "Facet: %s\n", _name.c_str());

    ext::optional<car::AttributeList> attributes = this->attributes();
    if (attributes) {
        attributes->dump();
    }
}

struct _car_facet_rendition_iterate_ctx {
    Facet const *facet;
    uint16_t facet_identifier;
    Facet::RenditionIterator iterator;
    void *ctx;
};

static void
_car_facet_rendition_iterator(car::Archive const *archive, car::AttributeList const &attributes, void *ctx)
{
    struct _car_facet_rendition_iterate_ctx *iterate_ctx = (struct _car_facet_rendition_iterate_ctx *)ctx;

    ext::optional<uint16_t> rendition_identifier = attributes.get(car_attribute_identifier_identifier);
    if (rendition_identifier && *rendition_identifier == iterate_ctx->facet_identifier) {
        ((Facet::RenditionIterator)iterate_ctx->iterator)(iterate_ctx->facet, attributes, iterate_ctx->ctx);
    }
}

void Facet::
renditionIterate(RenditionIterator iterator, void *ctx) const
{
    ext::optional<car::AttributeList> attributes = this->attributes();
    if (!attributes) {
        return;
    }

    ext::optional<uint16_t> facet_identifier = attributes->get(car_attribute_identifier_identifier);
    if (!facet_identifier) {
        return;
    }

    struct _car_facet_rendition_iterate_ctx iterate_ctx = {
        .facet = this,
        .facet_identifier = *facet_identifier,
        .iterator = iterator,
        .ctx = ctx,
    };
    _archive->renditionIterate(_car_facet_rendition_iterator, &iterate_ctx);
}

struct _car_facet_exists_ctx {
    std::string const *name;
    bool exists;
};

static void
_car_facet_exists_iterator(car::Archive const *context, std::string const &name, void *ctx)
{
    struct _car_facet_exists_ctx *exists_ctx = (struct _car_facet_exists_ctx *)ctx;
    exists_ctx->exists = exists_ctx->exists || name == *exists_ctx->name;
}

ext::optional<Facet> Facet::
Load(Archive const *archive, std::string const &name)
{
    /* Verify the facet exists. */
    struct _car_facet_exists_ctx ctx = { .name = &name, .exists = false };
    archive->facetIterate(_car_facet_exists_iterator, &ctx);
    if (!ctx.exists) {
        return ext::nullopt;
    }

    return Facet(archive, name);
}

ext::optional<Facet> Facet::
Create(Archive *archive, std::string const &name, AttributeList const &attributes)
{
    /* Verify facet does not yet exist. */
    struct _car_facet_exists_ctx ctx = { .name = &name, .exists = false };
    archive->facetIterate(_car_facet_exists_iterator, &ctx);
    if (ctx.exists) {
        return ext::nullopt;
    }

    size_t key_len = sizeof(car_facet_key) + name.size();
    car_facet_key *key = (car_facet_key *)malloc(key_len);
    if (key == NULL) {
        return ext::nullopt;
    }
    strncpy(key, name.c_str(), name.size());

    size_t attribute_count = attributes.count();
    size_t attribute_len = sizeof(struct car_facet_value) + sizeof(struct car_attribute_pair) * attribute_count;
    struct car_facet_value *value = (struct car_facet_value *)malloc(attribute_len);
    if (value == NULL) {
        free(key);
        return ext::nullopt;
    }
    value->hot_spot.x = 0; // todo
    value->hot_spot.y = 0; // todo
    value->attributes_count = attribute_count;

    size_t index = 0;
    attributes.iterate([&](enum car_attribute_identifier identifier, uint16_t attribute_value) {
        struct car_attribute_pair *attribute = &value->attributes[index];
        attribute->identifier = identifier;
        attribute->value = attribute_value;
    });

    struct bom_tree_context *tree = bom_tree_alloc_load(archive->bom(), car_facet_keys_variable);
    if (tree == NULL) {
        free(key);
        free(value);
        return ext::nullopt;
    }

    bom_tree_add(tree, key, key_len, value, attribute_len);

    free(key);
    free(value);
    bom_tree_free(tree);

    return Facet(archive, name);
}

