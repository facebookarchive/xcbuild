/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/car.h>
#include <car/car_format.h>

#include <string.h>
#include <assert.h>

struct car_facet_context {
    struct car_context *car;
    struct bom_tree_context *tree;
    char *name;
};

struct car_facet_context *
_car_facet_alloc(struct car_context *car, const char *name)
{
    assert(car != NULL);
    assert(name != NULL);

    struct car_facet_context *context = (struct car_facet_context *)malloc(sizeof(struct car_facet_context));
    if (context == NULL) {
        return NULL;
    }

    context->car = car;
    context->name = NULL;
    context->tree = NULL;

    context->name = strdup(name);
    if (context->name == NULL) {
        car_facet_free(context);
        return NULL;
    }

    context->tree = bom_tree_alloc_load(car_bom_get(context->car), car_facet_keys_variable);
    if (context->tree == NULL) {
        car_facet_free(context);
        return NULL;
    }

    return context;
}

struct _car_facet_exists_ctx {
    const char *name;
    bool exists;
};

static void
_car_facet_exists_iterator(struct car_context *context, const char *name, void *ctx)
{
    struct _car_facet_exists_ctx *exists_ctx = (struct _car_facet_exists_ctx *)ctx;
    exists_ctx->exists = exists_ctx->exists || !strcmp(name, exists_ctx->name);
}

struct car_facet_context *
car_facet_alloc_load(struct car_context *car, const char *name)
{
    struct car_facet_context *context = _car_facet_alloc(car, name);
    if (context == NULL) {
        return NULL;
    }

    /* Verify facet does, in fact, exist. */
    struct _car_facet_exists_ctx ctx = { .name = context->name, .exists = false };
    car_facet_iterate(context->car, _car_facet_exists_iterator, &ctx);
    if (!ctx.exists) {
        car_facet_free(context);
        return NULL;
    }

    return context;
}

struct car_facet_context *
car_facet_alloc_new(struct car_context *car, const char *name, car::AttributeList attributes)
{
    struct car_facet_context *context = _car_facet_alloc(car, name);
    if (context == NULL) {
        return NULL;
    }

    /* Verify facet does not yet exist. */
    struct _car_facet_exists_ctx ctx = { .name = context->name, .exists = false };
    car_facet_iterate(context->car, _car_facet_exists_iterator, &ctx);
    if (ctx.exists) {
        car_facet_free(context);
        return NULL;
    }

    size_t key_len = sizeof(car_facet_key) + strlen(name);
    car_facet_key *key = (car_facet_key *)malloc(key_len);
    if (key == NULL) {
        car_facet_free(context);
        return NULL;
    }
    strncpy(key, name, strlen(name));

    size_t attribute_count = attributes.count();
    size_t attribute_len = sizeof(struct car_facet_value) + sizeof(struct car_attribute_pair) * attribute_count;
    struct car_facet_value *value = (struct car_facet_value *)malloc(attribute_len);
    if (value == NULL) {
        free(key);
        car_facet_free(context);
        return NULL;
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

    bom_tree_add(context->tree, key, key_len, value, attribute_len);

    free(key);
    free(value);

    return context;
}

struct car_context *
car_facet_car_get(struct car_facet_context *context)
{
    assert(context != NULL);
    return context->car;
}

void
car_facet_free(struct car_facet_context *context)
{
    if (context == NULL) {
        return;
    }

    bom_tree_free(context->tree);
    free(context->name);
    free(context);
}

struct _car_facet_attributes_ctx {
    struct car_facet_context *context;
    ext::optional<car::AttributeList> attributes;
};

static void
_car_facet_attributes_iterator(struct bom_tree_context *tree, void *key, size_t key_len, void *value, size_t value_len, void *ctx)
{
    struct _car_facet_attributes_ctx *attributes_ctx = (struct _car_facet_attributes_ctx *)ctx;

    car_facet_key *facet_key = (car_facet_key *)key;
    if (!strncmp(attributes_ctx->context->name, facet_key, key_len)) {
        struct car_facet_value *facet_value = (struct car_facet_value *)value;

        enum car_attribute_identifier *attribute_identifiers = (enum car_attribute_identifier *)malloc(facet_value->attributes_count * sizeof(enum car_attribute_identifier));
        uint16_t *attribute_values = (uint16_t *)malloc(facet_value->attributes_count * sizeof(uint16_t));
        if (attribute_identifiers == NULL || attribute_values == NULL) {
            free(attribute_identifiers);
            free(attribute_values);
            return;
        }

        for (size_t i = 0; i < facet_value->attributes_count; i++) {
            struct car_attribute_pair *attribute = &facet_value->attributes[i];
            attribute_identifiers[i] = (enum car_attribute_identifier)attribute->identifier;
            attribute_values[i] = attribute->value;
        }

        attributes_ctx->attributes = car::AttributeList::Load(facet_value->attributes_count, attribute_identifiers, attribute_values);

        free(attribute_identifiers);
        free(attribute_values);
    }
}

ext::optional<car::AttributeList>
car_facet_attributes_copy(struct car_facet_context *context)
{
    assert(context != NULL);

    struct bom_tree_context *tree = bom_tree_alloc_load(car_bom_get(context->car), car_facet_keys_variable);
    if (tree == NULL) {
        return ext::nullopt;
    }

    struct _car_facet_attributes_ctx ctx = {
        .context = context,
        .attributes = ext::nullopt,
    };
    bom_tree_iterate(tree, _car_facet_attributes_iterator, &ctx);
    bom_tree_free(tree);

    /* Will be ext::nullopt if not found. */
    return ctx.attributes;
}

struct _car_facet_rendition_iterate_ctx {
    struct car_facet_context *context;
    car::AttributeList attributes;
    car_facet_rendition_iterator iterator;
    void *ctx;
};

static void
_car_facet_rendition_iterator(struct car_context *context, car::AttributeList const &attributes, void *ctx)
{
    struct _car_facet_rendition_iterate_ctx *iterate_ctx = (struct _car_facet_rendition_iterate_ctx *)ctx;

    ext::optional<uint16_t> facet_identifier = iterate_ctx->attributes.get(car_attribute_identifier_identifier);
    ext::optional<uint16_t> rendition_identifier = attributes.get(car_attribute_identifier_identifier);
    if (facet_identifier && rendition_identifier && *facet_identifier == *rendition_identifier) {
        ((car_facet_rendition_iterator)iterate_ctx->iterator)(iterate_ctx->context, attributes, iterate_ctx->ctx);
    }
}

void
car_facet_rendition_iterate(struct car_facet_context *context, car_facet_rendition_iterator iterator, void *ctx)
{
    assert(context != NULL);

    ext::optional<car::AttributeList> attributes = car_facet_attributes_copy(context);
    if (!attributes) {
        return;
    }

    struct _car_facet_rendition_iterate_ctx iterate_ctx = {
        .context = context,
        .attributes = *attributes,
        .iterator = iterator,
        .ctx = ctx,
    };
    car_rendition_iterate(context->car, _car_facet_rendition_iterator, &iterate_ctx);
}

