/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */
/*
 * Copyright Nils Alexander Roemcke 2005.
 *
 * Use, modification, and distribution are subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */


#include "parser.h"
#include "tokenizer.h"

#include "json_internal.h"

static void
wrong_type_string(struct parse_state_t *ps)
{
    puts("string not allowed");
}

static void
wrong_type_null(struct parse_state_t *ps)
{
    puts("null not allowed");
}

static void
wrong_type_bool(struct parse_state_t *ps)
{
    puts("bool not allowed");
}

static void
wrong_type_int(struct parse_state_t *ps)
{
    puts("int not allowed");
}

static void
wrong_type_double(struct parse_state_t *ps)
{
    puts("double not allowed");
}

static void
wrong_type_compound(struct parse_state_t *ps, enum context_id_t p)
{
    puts("array/object not allowed");
}

int
json_parse(const char *fn,
           struct json_cb_t *vb,
           void *root,
           json_err_cb *err_cb,
           void *err_data)
{
    FILE *f = fopen(fn, "r");
    if (!f)
        return -1;

    int rc = json_fparse(f, vb, root, err_cb, err_data);
    fclose(f);
    return rc;
}

static struct context_t *
context_push(struct context_t *c)
{
    struct context_t *retval = calloc(1, sizeof(struct context_t));
    if (retval) {
        retval->prev = c;
    }

    return retval;
}

struct context_t *
_json_context_push(struct context_t *c,
                   enum context_id_t p,
                   struct json_cb_t *cb,
                   void *data)
{
    struct context_t *retval = context_push(c);
    if (retval) {
        retval->ctype = p;
        retval->data = data;
        retval->vtbl = cb;
    }
    return retval;
}

struct context_t *
_json_context_push_null(struct context_t *c, enum context_id_t p)
{
    struct context_t *retval = _json_context_push(c, p, NULL, NULL);

    return retval;
}

struct context_t *
_json_context_push_root(struct context_t *c, struct json_cb_t *cb, void *data)
{
    struct context_t *retval = context_push(c);
    if (retval) {
        retval->ctype = context_root;
        retval->data = data;
        retval->vtbl = cb;
    }

    return retval;
}

struct context_t *
_json_context_push_obj(struct context_t *c, struct json_cb_t *cb, void *data)
{
    struct context_t *retval = context_push(c);
    if (retval) {
        retval->ctype = context_obj;
        retval->data = data;
        retval->vtbl = cb;
    }

    return retval;
}

struct context_t *
_json_context_push_array(struct context_t *c, struct json_cb_t *cb, void *data)
{
    struct context_t *retval = context_push(c);
    if (retval) {
        retval->ctype = context_array;
        retval->data = data;
        retval->vtbl = cb;
    }

    return retval;
}

struct context_t *
_json_context_pop(struct context_t *c)
{
    struct context_t *retval = c->prev;

    if (c->member) {
        free(c->member);
    }

    if (c->data) {
        if (c->vtbl && c->vtbl->delete_cb)
            (c->vtbl->delete_cb)(c->data);
    }

    free(c);

    return retval;
}


void
_json_context_popall(struct context_t *c)
{
    while (c) {
        c = _json_context_pop(c);
    }
}

void
_json_parse_member(struct parse_state_t *ps, char *m)
{
    if (ps->context->member) {
        free(ps->context->member);
    }

    ps->context->member = m;
}

static char *
get_member(struct context_t *c)
{
    if (c->ctype != context_obj)
        return NULL;

    return c->member;
}

int
_json_parse_string(struct parse_state_t *ps, char *s)
{
    int err = -1;
    struct context_t *c = ps->context;
    const char *member = get_member(c);

    if (!c->vtbl)  {
        err = 0;
    } else if (c->vtbl->string_cb) {
        err = (c->vtbl->string_cb)(c->data, member, s);
    }

    if (err) {
        wrong_type_string(ps);
    }

    free(s);
    return err;
}

static unsigned char
hex_to_bin_digit(char ch)
{
    if (ch >= 'a' && ch <= 'f')
        return (ch - 'a') + 10;
    else if (ch >= 'A' && ch <= 'F')
        return (ch - 'A') + 10;
    else if (ch >= '0' && ch <= '9')
        return (ch - '0');
    else
        return 0;
}


static unsigned char
hex_to_bin(char const *ascii)
{
    return (hex_to_bin_digit(ascii[0]) << 4) | hex_to_bin_digit(ascii[1]);
}

int
_json_parse_data(struct parse_state_t *ps, char *hexdata)
{
    int err = -1;
    struct context_t *c = ps->context;
    const char *member = get_member(c);

    if (!c->vtbl)  {
        err = 0;
    } else if (c->vtbl->data_cb) {
        size_t         n, alength = strlen(hexdata);
        size_t         length = alength / 2;
        unsigned char *data = (unsigned char *)calloc(length, 1);
        if (data == NULL)
            return err;

        for (n = 0; n < alength; n += 2) {
            data[n >> 1] = hex_to_bin(hexdata + n);
        }

        err = (c->vtbl->data_cb)(c->data, member, data, length);
        free(data);
    }

    if (err) {
        wrong_type_string(ps);
    }

    free(hexdata);
    return err;
}

int
_json_parse_null(struct parse_state_t *ps)
{
    struct context_t *c = ps->context;
    const char *member = get_member(c);

    if (!c->vtbl)
        return 0;

    if (c->vtbl->null_cb) {
        return (c->vtbl->null_cb)(c->data, member);
    }

    wrong_type_null(ps);

    return -1;
}


#define J_PARSE(T, M, CT)                           \
int                                                 \
_json_parse_ ## T (struct parse_state_t *ps, CT i)  \
{                                                   \
    struct context_t *c = ps->context;              \
    int err = -1;                                   \
                                                    \
    if (!c->vtbl)                                   \
        return 0;                                   \
                                                    \
    const char *member = get_member(c);             \
                                                    \
    if (c->vtbl->M) {                               \
        err = (c->vtbl->M)(c->data, member, i);     \
    }                                               \
                                                    \
    if (err) {                                      \
        wrong_type_ ## T(ps);                       \
    }                                               \
                                                    \
    return err;                                     \
}

J_PARSE(bool, bool_cb, int)
J_PARSE(int, int_cb, int)
J_PARSE(double, double_cb, double)

int
_json_parse_obj_start(struct parse_state_t *ps)
{
    return _json_parse_compound_start(ps, context_obj);
}

int
_json_parse_obj_end(struct parse_state_t *ps)
{
    return _json_parse_compound_end(ps);
}

int
_json_parse_array_start(struct parse_state_t *ps)
{
    return _json_parse_compound_start(ps, context_array);
}

int
_json_parse_array_end(struct parse_state_t *ps)
{
    return _json_parse_compound_end(ps);
}

int
_json_parse_compound_start(struct parse_state_t *ps, enum context_id_t p)
{
    int err = -1;
    struct context_t *c = ps->context;
    const char *member = get_member(c);
    struct context_t *c_pushed;
    void *data = NULL;
    struct json_cb_t *cb = NULL;

    if (!c->vtbl) {
        err = 0;
    } else if (c->vtbl->new_obj_cb && p == context_obj) {
        err = (c->vtbl->new_obj_cb)(c->data, member, &data, &cb);
    } else if (c->vtbl->new_array_cb && p == context_array) {
        err = (c->vtbl->new_array_cb)(c->data, member, &data, &cb);
    }

    if (err) {
        wrong_type_compound(ps, p);
    }

    if (!err && cb) {
        c_pushed = _json_context_push(c, p, cb, data);
    } else {
        c_pushed = _json_context_push_null(c, p);
    }

    if (c_pushed) {
        ps->context = c_pushed;
    } else {
        // TODO:
        //  -handle no mem
    }

    return err;
}

int
_json_parse_compound_end(struct parse_state_t *ps)
{
    int err = -1;
    struct context_t *cc = ps->context;
    struct context_t *c = cc->prev;
    void *data = cc->data;

    if (c->vtbl && c->vtbl->obj_cb) {
        err = (c->vtbl->obj_cb)(c->data, data);
    }

    if (!err) {
        cc->data = NULL;
    }

    ps->context = _json_context_pop(cc);

    return err;
}
