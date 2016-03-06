/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _CAR_H
#define _CAR_H

#include <stdlib.h>
#include <stdbool.h>

#include <bom/bom.h>

#include <car/Archive.h>
#include <car/AttributeList.h>
#include <car/Facet.h>


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
car_rendition_alloc_load(car::Archive *car, car::AttributeList const &attributes);

struct car_rendition_context *
car_rendition_alloc_new(car::Archive *car, car::AttributeList const &attributes, struct car_rendition_properties properties, void *data, size_t data_len);

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
car_key_format_dump(car::Archive const *context);

void
car_header_dump(car::Archive const *context);

void
car_part_dump(car::Archive const *context);

void
car_element_dump(car::Archive const *context);


#endif /* _CAR_H */
