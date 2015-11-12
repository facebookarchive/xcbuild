/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/ASCIIParser.h>

#include <plist/Objects.h>

#include <jsoncb.h>

using plist::ASCIIParser;

using namespace plist;

ASCIIParser::ASCIIParser()
{
}

static int _json_string(void *obj, char const *m, char const *s);
static int _json_data(void *obj, char const *m, unsigned char const *buf, size_t len);
static int _json_int(void *obj, char const *m, int i);
static int _json_double(void *obj, char const *m, double d);
static int _json_bool(void *obj, char const *m, int b);
static int _json_null(void *obj, char const *m);
static int _json_new_obj(void *obj, char const *m, void **obj_new, struct json_cb_t **cb);
static int _json_obj(void *obj, void *o);
static int _json_new_array(void *obj, char const *m, void **array_new, struct json_cb_t **cb);
static int _json_array(void *obj, void *a);
static void *_json_delete(void *obj);

static struct json_cb_t _json_vb = {
    _json_string,
    _json_data,
    _json_int,
    _json_double,
    _json_bool,
    _json_null,

    _json_new_obj,
    _json_obj,

    _json_new_array,
    _json_array,

    _json_delete
};

static int
_json_string(void *obj, char const *m, char const *s)
{
    if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(String::New(s));
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, String::New(s));
    } else {
        assert(0 && "this shouldn't happen");
    }
    return 0;
}

static int
_json_data(void *obj, char const *m, unsigned char const *buf, size_t len)
{
    if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(Data::New(buf, len));
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, Data::New(buf, len));
    } else {
        assert(0 && "this shouldn't happen");
    }
    return 0;
}

static int
_json_int(void *obj, char const *m, int i)
{
    if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(Integer::New(i));
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, Integer::New(i));
    } else {
        assert(0 && "this shouldn't happen");
    }
    return 0;
}

static int
_json_double(void *obj, char const *m, double r)
{
    if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(Real::New(r));
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, Real::New(r));
    } else {
        assert(0 && "this shouldn't happen");
    }
    return 0;
}

static int
_json_bool(void *obj, char const *m, int b)
{
    if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(Boolean::New(b != 0));
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, Boolean::New(b != 0));
    } else {
        assert(0 && "this shouldn't happen");
    }
    return 0;
}

static int
_json_null(void *obj, char const *m)
{
    if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(Null::New());
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, Null::New());
    } else {
        assert(0 && "this shouldn't happen");
    }
    return 0;
}

static int
_json_new_obj(void *obj, char const *m, void **obj_new, struct json_cb_t **cb)
{
    Dictionary *dict = Dictionary::New();
    if (m == nullptr) {
        if (*reinterpret_cast <Object **> (obj) == nullptr) {
            *reinterpret_cast <Object **> (obj) = dict;
        } else if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
            a->append(dict);
        } else {
            assert(0 && "this shouldn't happen");
        }
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, dict);
    } else {
        assert(0 && "this shouldn't happen");
    }
    *obj_new = dict;
    *cb      = &_json_vb;
    return 0;
}

static int
_json_obj(void *obj, void *o)
{
    return 0;
}

static int
_json_new_array(void *obj, char const *m, void **array_new, struct json_cb_t **cb)
{
    Array *array = Array::New();
    if (*reinterpret_cast <Object **> (obj) == nullptr) {
        *reinterpret_cast <Object **> (obj) = array;
    } else if (Array *a = CastTo <Array> (reinterpret_cast <Object *> (obj))) {
        a->append(array);
    } else if (Dictionary *d = CastTo <Dictionary> (reinterpret_cast <Object *> (obj))) {
        assert(m != nullptr && "key is null");
        d->set(m, array);
    } else {
        assert(0 && "this shouldn't happen");
    }
    *array_new = array;
    *cb        = &_json_vb;
    return 0;
}

static int
_json_array(void *obj, void *a)
{
    return 0;
}

static void *
_json_delete(void *obj)
{
    return 0;
}

struct ErrorContext {
    error_function const &ef;

    ErrorContext(error_function const &func) :
        ef(func)
    { }
};

static int
_json_err(void *err_data, unsigned int line, unsigned int column, char const *error)
{
    ErrorContext *ctx = reinterpret_cast <ErrorContext *> (err_data);

    ctx->ef(line, column, error);

    return -1;
}

Object *ASCIIParser::
parse(std::string const &path, error_function const &error)
{
    Object       *root = nullptr;
    ErrorContext  ectx(error);

    if (json_parse(path.c_str(), &_json_vb, &root, _json_err, &ectx) != 0) {
        if (root != nullptr) {
            root->release();
        }
        root = nullptr;
    }

    return root;
}

Object *ASCIIParser::
parse(std::FILE *fp, error_function const &error)
{
    Object       *root = nullptr;
    ErrorContext  ectx(error);

    if (json_fparse(fp, &_json_vb, &root, _json_err, &ectx) != 0) {
        if (root != nullptr) {
            root->release();
        }
        root = nullptr;
    }

    return root;
}

