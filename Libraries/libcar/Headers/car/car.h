/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _CAR_H
#define _CAR_H

#include <stdlib.h>
#include <stdbool.h>

#include <bom/bom.h>

#include <car/Reader.h>
#include <car/AttributeList.h>
#include <car/Facet.h>
#include <car/Rendition.h>

void
car_key_format_dump(car::Reader const *context);

void
car_header_dump(car::Reader const *context);

void
car_part_dump(car::Reader const *context);

void
car_element_dump(car::Reader const *context);


#endif /* _CAR_H */
