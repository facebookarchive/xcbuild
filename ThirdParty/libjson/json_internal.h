/*
 * Copyright 2013-present Facebook. All rights reserved.
 * Copyright Nils Alexander Roemcke 2005.
 *
 * Use, modification, and distribution are subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef JSON_JSON_INTERNAL_H_4137523a_6149_4624_92e4_d8dff36075c0
#define JSON_JSON_INTERNAL_H_4137523a_6149_4624_92e4_d8dff36075c0

#include <setjmp.h>

#include "jsoncb.h"

enum context_id_t {
    context_none = 0,
    context_root = 1,
    context_obj = 2,
    context_array = 3
};

struct context_t {
    struct context_t *prev;

    enum context_id_t ctype;

    char *member;
    void *data;

    struct json_cb_t  *vtbl;
};

struct parse_state_t {
    json_err_cb *err_cb;
    void *err_data;
    jmp_buf jb;

    struct context_t *context;
};

struct context_t *
_json_context_push(struct context_t *c,
                   enum context_id_t p,
                   struct json_cb_t *cb,
                   void *data);

struct context_t *
_json_context_push_null(struct context_t *c, enum context_id_t p);

struct context_t *
_json_context_push_root(struct context_t *c, struct json_cb_t *cb, void *data);

struct context_t *
_json_context_push_obj(struct context_t *c, struct json_cb_t *cb, void *data);

struct context_t *
_json_context_push_array(struct context_t *c, struct json_cb_t *cb, void *data);

struct context_t *
_json_context_pop(struct context_t *c);

void
_json_context_popall(struct context_t *c);

int
_json_parse_string(struct parse_state_t *ps, char *s);

int
_json_parse_null(struct parse_state_t *ps);

int
_json_parse_bool(struct parse_state_t *ps, int i);

int
_json_parse_int(struct parse_state_t *ps, int i);

int
_json_parse_double(struct parse_state_t *ps, double i);

int
_json_parse_obj(struct parse_state_t *ps);

int
_json_parse_array(struct parse_state_t *ps);

int
_json_parse_obj_start(struct parse_state_t *ps);

int
_json_parse_obj_end(struct parse_state_t *ps);

int
_json_parse_array_start(struct parse_state_t *ps);

int
_json_parse_array_end(struct parse_state_t *ps);

int
_json_parse_data(struct parse_state_t *ps, char *hexdata);

int
_json_parse_compound_start(struct parse_state_t *ps, enum context_id_t p);

int
_json_parse_compound_end(struct parse_state_t *ps);

void
_json_parse_member(struct parse_state_t *ps, char *m);

#endif /* JSON_JSON_INTERNAL_H_4137523a_6149_4624_92e4_d8dff36075c0 */
