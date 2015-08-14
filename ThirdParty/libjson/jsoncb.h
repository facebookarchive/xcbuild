/*
 * Copyright 2013-present Facebook. All rights reserved.
 * Copyright Nils Alexander Roemcke 2005.
 *
 * Use, modification, and distribution are subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef JSON_JSON_H_94e2ebdd_cb8c_4331_b448_a416758d0669
#define JSON_JSON_H_94e2ebdd_cb8c_4331_b448_a416758d0669

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Forward declarations
 */

struct json_cb_t;

/*
 * Callbacks
 */

typedef int json_string_cb(void *obj, const char *m, const char *s);
typedef int json_data_cb(void *obj, const char *m, const unsigned char *buf, size_t len);
typedef int json_int_cb(void *obj, const char *m, int i);
typedef int json_double_cb(void *obj, const char *m, double d);
typedef int json_bool_cb(void *obj, const char *m, int b);
typedef int json_null_cb(void *obj, const char *m);

typedef int json_new_obj_cb(void *obj, const char *m, void **obj_new, struct json_cb_t **cb);
typedef int json_obj_cb(void *obj, void *o);

typedef int json_new_array_cb(void *obj, const char *m, void **array_new, struct json_cb_t **cb);
typedef int json_array_cb(void *obj, void *a);

typedef void *json_delete_cb(void *obj);

struct json_cb_t {
    json_string_cb    *string_cb;
    json_data_cb      *data_cb;
    json_int_cb       *int_cb;
    json_double_cb    *double_cb;
    json_bool_cb      *bool_cb;
    json_null_cb      *null_cb;

    json_new_obj_cb   *new_obj_cb;
    json_obj_cb       *obj_cb;

    json_new_array_cb *new_array_cb;
    json_array_cb     *array_cb;

    json_delete_cb    *delete_cb;
};

/*
 * Error handling
 */

typedef int json_err_cb(void *err_data, unsigned int line, unsigned int column, const char *error);

/*
 * Parser functions
 */

int
json_fparse(FILE *f, struct json_cb_t *vb, void *root, json_err_cb *err_cb, void *err_data);

int
json_parse(const char *fn, struct json_cb_t *vb, void *root, json_err_cb *err_cb, void *err_data);

#ifdef __cplusplus
}
#endif

#endif /* JSON_JSON_H_94e2ebdd_cb8c_4331_b448_a416758d0669 */
