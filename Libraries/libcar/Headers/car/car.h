/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _CAR_H
#define _CAR_H

#include <stdlib.h>
#include <stdbool.h>

#include <bom/bom.h>

/* Unfortunate, but needed for car_attribute_identifier. */
#include <car/car_format.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Attributes */

struct car_attribute_list;

struct car_attribute_list *
car_attribute_alloc_values(size_t count, enum car_attribute_identifier *identifiers, uint16_t *values);

struct car_attribute_list *
car_attribute_alloc_copy(struct car_attribute_list *attributes);

uint16_t
car_attribute_get(struct car_attribute_list *attributes, enum car_attribute_identifier identifier);

void
car_attribute_set(struct car_attribute_list *attributes, enum car_attribute_identifier identifier, uint16_t value);

bool
car_attribute_equal(struct car_attribute_list *attributes1, struct car_attribute_list *attributes2);

size_t
car_attribute_count(struct car_attribute_list *attributes);

typedef void (*car_attribute_iterator)(struct car_attribute_list *attributes, int index, enum car_attribute_identifier identifier, uint16_t value, void *ctx);

void
car_attribute_iterate(struct car_attribute_list *attributes, car_attribute_iterator iterator, void *ctx);

void
car_attribute_dump(struct car_attribute_list *attributes);

void
car_attribute_free(struct car_attribute_list *attributes);


/* Asset Archive */

struct car_context;

struct car_context *
car_alloc_load(struct bom_context *bom);

struct car_context *
car_alloc_empty(struct bom_context *bom);

void
car_free(struct car_context *context);

struct bom_context *
car_bom_get(struct car_context *context);

typedef void (*car_rendition_iterator)(struct car_context *context, struct car_attribute_list *attributes, void *ctx);

void
car_rendition_iterate(struct car_context *context, car_rendition_iterator iterator, void *ctx);

typedef void (*car_facet_iterator)(struct car_context *context, const char *name, void *ctx);

void
car_facet_iterate(struct car_context *context, car_facet_iterator iterator, void *ctx);


/* Rendition */

struct car_rendition_context;

struct car_rendition_properties {
    char file_name[129];
    int modification_time;

    int width;
    int height;
    float scale;
};

struct car_rendition_context *
car_rendition_alloc_load(struct car_context *car, struct car_attribute_list *attributes);

struct car_rendition_context *
car_rendition_alloc_new(struct car_context *car, struct car_attribute_list *attributes, struct car_rendition_properties properties, void *data, size_t data_len);

struct car_context *
car_rendition_car_get(struct car_rendition_context *context);

void
car_rendition_dump(struct car_rendition_context *context);

void
car_rendition_free(struct car_rendition_context *context);

struct car_attribute_list *
car_rendition_attributes_copy(struct car_rendition_context *context);

void *
car_rendition_data_copy(struct car_rendition_context *context, size_t *data_len);

struct car_rendition_properties
car_rendition_properties_get(struct car_rendition_context *context);


/* Facet */

struct car_facet_context;

struct car_facet_context *
car_facet_alloc_load(struct car_context *car, const char *name);

struct car_facet_context *
car_facet_alloc_new(struct car_context *car, const char *name, struct car_attribute_list *attributes);

struct car_context *
car_facet_car_get(struct car_facet_context *context);

void
car_facet_free(struct car_facet_context *context);

struct car_attribute_list *
car_facet_attributes_copy(struct car_facet_context *context);

typedef void (*car_facet_rendition_iterator)(struct car_facet_context *context, struct car_attribute_list *attributes, void *ctx);

void
car_facet_rendition_iterate(struct car_facet_context *context, car_facet_rendition_iterator iterator, void *ctx);


/* Debug */

void
car_key_format_dump(struct car_context *context);

void
car_header_dump(struct car_context *context);

void
car_part_dump(struct car_context *context);

void
car_element_dump(struct car_context *context);


#ifdef __cplusplus
}
#endif

#endif /* _CAR_H */
