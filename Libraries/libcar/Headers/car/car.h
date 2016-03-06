/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _CAR_H
#define _CAR_H

#include <stdlib.h>
#include <stdbool.h>

#include <bom/bom.h>

#include <car/AttributeList.h>
#include <car/Facet.h>


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

typedef void (*car_rendition_iterator)(struct car_context *context, car::AttributeList const &attributes, void *ctx);

void
car_rendition_iterate(struct car_context *context, car_rendition_iterator iterator, void *ctx);

typedef void (*car_facet_iterator)(struct car_context *context, car::Facet const &facet, void *ctx);

void
car_facet_iterate(struct car_context *context, car_facet_iterator iterator, void *ctx);

typedef void (*car_facet_rendition_iterator)(struct car_context *context, car::Facet const *facet, car::AttributeList const &attributes, void *ctx);

void
car_facet_rendition_iterate(struct car_context *context, car::Facet const &facet, car_facet_rendition_iterator iterator, void *ctx);

bool
car_facet_add(struct car_context *context, car::Facet const &facet);


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
car_rendition_alloc_load(struct car_context *car, car::AttributeList const &attributes);

struct car_rendition_context *
car_rendition_alloc_new(struct car_context *car, car::AttributeList const &attributes, struct car_rendition_properties properties, void *data, size_t data_len);

struct car_context *
car_rendition_car_get(struct car_rendition_context *context);

void
car_rendition_dump(struct car_rendition_context *context);

void
car_rendition_free(struct car_rendition_context *context);

car::AttributeList
car_rendition_attributes_copy(struct car_rendition_context *context);

void *
car_rendition_data_copy(struct car_rendition_context *context, size_t *data_len);

struct car_rendition_properties
car_rendition_properties_get(struct car_rendition_context *context);



/* Debug */

void
car_key_format_dump(struct car_context *context);

void
car_header_dump(struct car_context *context);

void
car_part_dump(struct car_context *context);

void
car_element_dump(struct car_context *context);


#endif /* _CAR_H */
