/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/car_format.h>

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

