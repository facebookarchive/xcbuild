// Copyright 2013-present Facebook. All Rights Reserved.

#include <plist/Objects.h>
#include <plist/SimpleXMLParser.h>
#include <plist/XMLParser.h>
#include <plist/BinaryParser.h>

#include <jsoncb.h>

#include <cassert>
#include <cstring>
#include <csetjmp>
#include <cerrno>

using namespace plist;

Null const Null::kNull;
Boolean const Boolean::kTrue(true);
Boolean const Boolean::kFalse(false);

//
// Object
//

char const *Object::
GetTypeName(enum Object::Type type)
{
    switch (type) {
        case kTypeInteger:    return "integer";
        case kTypeReal:       return "real";
        case kTypeString:     return "string";
        case kTypeBoolean:    return "boolean";
        case kTypeNull:       return "null";
        case kTypeArray:      return "array";
        case kTypeDictionary: return "dictionary";
        case kTypeData:       return "data";
        case kTypeDate:       return "date";
        default:              break;
    }
    return "unknown";
}

Object const *Object::
traverse(std::string const &path) const
{
    char const   *cpath;
    char const   *bpath;
    Object const *obj = this;

    if (obj == nullptr)
        return nullptr;

    if (path.empty())
        return this;

    cpath = path.c_str();

    while (*cpath != '\0') {
        if (*cpath == '[') {
            if (Array const *a = CastTo <Array> (obj)) {
                char *end;
                unsigned long index = strtoul(cpath + 1, &end, 0);
                if (index >= a->count())
                    return nullptr;
                if (*end != ']')
                    return nullptr;

                obj = a->value(index);
                cpath = end + 1;
            } else {
                return nullptr;
            }
        } else if (obj == this || *cpath == '.') {
            if (Dictionary const *d = CastTo <Dictionary> (obj)) {
                if (*cpath == '.') {
                    cpath++;
                }
                bpath = cpath;
                while (*cpath != '\0') {
                    if (*cpath == '.' || *cpath == '[') {
                        if (cpath == bpath || *(cpath - 1) != '\\')
                            break;
                    }

                    cpath++;
                }

                obj = d->value(std::string(bpath, cpath - bpath));
                if (obj == nullptr)
                    return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    return obj;
}

void Object::
dump(FILE *fp, size_t indent) const
{
    if (fp != nullptr) {
        dump1(fp, indent, indent);
        fputc('\n', fp);
    }
}

inline void Object::
Indent(FILE *fp, size_t n)
{
    fprintf(fp, "%*s", static_cast <int> (n * 4), "");
}

inline std::string Object::
QuoteString(std::string const &s)
{
    std::string o;

    for (size_t n = 0; n < s.length(); n++) {
        if (static_cast <uint8_t> (s[n]) < 32) {
            o += '\\';
            o += "0123456789abcdef"[(s[n] >> 4) & 0xf];
            o += "0123456789abcdef"[(s[n] >> 0) & 0xf];
        } else {
            if (s[n] == '\"' || s[n] == '\\') {
                o += '\\';
            }
            o += s[n];
        }
    }

    return o;
}

//
// Integer
//

Object *Integer::
copy() const
{
    return new Integer(value());
}

void Integer::
dump1(FILE *fp, size_t indent, size_t) const
{
    Object::Indent(fp, indent);
#ifdef _MSC_VER
    fprintf(fp, "%I64d", (__int64)value());
#else
    fprintf(fp, "%lld", (long long)value());
#endif
}

//
// Real
//

Object *Real::
copy() const
{
    return new Real(value());
}

void Real::
dump1(FILE *fp, size_t indent, size_t) const
{
    Object::Indent(fp, indent);
    fprintf(fp, "%g", value());
}

//
// String
//

Object *String::
copy() const
{
    return new String(value());
}

void String::
dump1(FILE *fp, size_t indent, size_t) const
{
    Object::Indent(fp, indent);
    fprintf(fp, "\"%s\"", Object::QuoteString(value()).c_str());
}

//
// Boolean
//

bool Boolean::
equals(Object const *obj) const
{
    if (Object::equals(obj))
        return true;

    Boolean const *objt = CastTo <Boolean> (obj);
    return (objt != nullptr && equals(objt));
}

Object *Boolean::
copy() const
{
    return const_cast <Boolean *> (this);
}

void Boolean::
dump1(FILE *fp, size_t indent, size_t) const
{
    Object::Indent(fp, indent);
    fprintf(fp, "%s", value() ? "true" : "false");
}

Boolean *
Boolean::New(Object const *object)
{
    if (object->type() == Type())
        return static_cast <Boolean *> (const_cast <Object *> (object));
    if (auto s = CastTo <String> (object)) {
        if (::strcasecmp(s->value().c_str(), "yes") == 0)
            return Boolean::New(true);
        if (::strcasecmp(s->value().c_str(), "no") == 0)
            return Boolean::New(false);
    }

    return nullptr;
}

//
// Null
//

Object *Null::
copy() const
{
    return const_cast <Null *> (this);
}

void Null::
dump1(FILE *fp, size_t indent, size_t) const
{
    Object::Indent(fp, indent);
    fprintf(fp, "null");
}

//
// Array
//

Object *Array::
copy() const
{
    Array *result = new Array;
    for (size_t n = 0; n < count(); n++) {
        result->append(value(n)->copy());
    }
    return result;
}

void Array::
merge(Array const *array)
{
    if (array == nullptr || array == this)
        return;

    for (auto obj : *array) {
        append(obj->copy());
    }
}

void Array::
dump1(FILE *fp, size_t, size_t cindent) const
{
    fputc('[', fp);
    fputc(empty() ? ' ' : '\n', fp);

    for (auto i = begin(); i != end(); ++i) {
        if (i != begin()) {
            fprintf(fp, ",\n");
        }
        switch ((*i)->type()) {
            case Object::kTypeArray:
            case Object::kTypeDictionary:
                Object::Indent(fp, cindent + 1);
                break;
            default:
                break;
        }
        (*i)->dump1(fp, cindent + 1, cindent + 1);
    }

    if (!empty()) {
        fputc('\n', fp);
        Object::Indent(fp, cindent);
    }

    fputc(']', fp);
}

//
// Dictionary
//

Object *Dictionary::
copy() const
{
    Dictionary *result = new Dictionary;
    for (size_t n = 0; n < count(); n++) {
        result->set(key(n), value(n)->copy());
    }
    return result;
}

void Dictionary::
merge(Dictionary const *dict, bool replace)
{
    if (dict == nullptr || dict == this)
        return;

    for (auto key : *dict) {
        if (replace || _map.find(key) == _map.end()) {
            set(key, dict->value(key)->copy());
        }
    }
}

void Dictionary::
dump1(FILE *fp, size_t, size_t cindent) const
{
    fputc('{', fp);
    fputc(empty() ? ' ' : '\n', fp);

    for (auto i = begin(); i != end(); ++i) {
        if (i != begin()) {
            fprintf(fp, ",\n");
        }
        Object::Indent(fp, cindent + 1);
        fprintf(fp, "\"%s\" : ", Object::QuoteString(*i).c_str());
        value(*i)->dump1(fp, 0, cindent + 1);
    }

    if (!empty()) {
        fputc('\n', fp);
        Object::Indent(fp, cindent);
    }

    fputc('}', fp);
}

//
// Data
//

Object *Data::
copy() const
{
    return new Data(_value);
}

void Data::
dump1(FILE *fp, size_t indent, size_t cindent) const
{
    bool   multiline     = false;
    size_t digitsPerLine = (((70 - (indent * 4) - 2) + 1) & -2) / 2;

    Object::Indent(fp, indent);
    fputc('<', fp);
    if (_value.empty()) {
        fputc(' ', fp);
    } else if (_value.size() * 2 >= digitsPerLine) {
        multiline = true;
        digitsPerLine = (((70 - ((cindent + 1) * 4) - 2) + 1) & -2) / 2;
        fputc('\n', fp);
    }

    if (digitsPerLine < 2) {
        digitsPerLine = 2;
    }

    for (size_t m, n = 0; n < _value.size();) {
        if (n != 0) {
            fputc('\n', fp);
        }
        if (multiline) {
            Object::Indent(fp, cindent + 1);
        }
        for (m = 0; m < digitsPerLine && (n + m) < _value.size(); m++) {
            if (n != 0) {
                fputc(' ', fp);
            }
            fprintf(fp, "%02x", _value[n + m]);
        }

        n += m;
    }

    if (!_value.empty() && multiline) {
        fputc('\n', fp);
        Object::Indent(fp, cindent);
    }

    fputc('>', fp);
}

//
// Date
//

Object *Date::
copy() const
{
    return new Date(_value);
}

void Date::
dump1(FILE *fp, size_t, size_t cindent) const
{
    char buf[256];
    strftime(buf, sizeof(buf), "%c", &_value);
    fprintf(fp, "%s", buf);
}

//
// Dictionary::Parser
//

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

namespace {

static Object *
JSONParse(std::string const &path, error_function const &error)
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

static Object *
JSONParse(std::FILE *fp, error_function const &error)
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

static Object *
XMLParse(std::FILE *fp, error_function const &error)
{
    return XMLParser().parse(fp, error);
}

static Object *
XMLParse(std::string const &path, error_function const &error)
{
    return XMLParser().parse(path, error);
}

static Object *
BinaryParse(std::string const &path, error_function const &error)
{
    return BinaryParser().parse(path, error);
}

static Object *
BinaryParse(std::FILE *fp, error_function const &error)
{
    return BinaryParser().parse(fp, error);
}

enum Format {
    kFormatSystemError = -1,
    kFormatUnknown,
    kFormatJSONOpenstep,
    kFormatXML,
    kFormatBinary
};

static Format
Identify(std::FILE *fp)
{
    char   buf[1024];
    size_t nread;

    ::rewind(fp);
    nread = ::fread(buf, 1, sizeof(buf), fp);
    ::rewind(fp);

    Format format = kFormatUnknown;
    if (nread != 0) {
        if (std::memcmp(buf, ABPLIST_MAGIC, ABPLIST_MAGIC_LENGTH) == 0) {
            format = kFormatBinary;
        } else {
            for (size_t n = 0; n < 1024; n++) {
                if (isspace(buf[n]))
                    continue;

                if (buf[n] == '(' || buf[n] == '{' || buf[n] == '/') {
                    format = kFormatJSONOpenstep;
                    break;
                } else if (buf[n] == '<') {
                    format = kFormatXML;
                    break;
                } else {
                    break;
                }
            }
        }
    }

    return format;
}

static Format
Identify(std::string const &path)
{
    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if (fp == nullptr)
        return kFormatSystemError;

    Format format = Identify(fp);
    std::fclose(fp);

    return format;
}

}

//
// Generic Parsing
//

Object *Object::
Parse(std::string const &path, error_function const &error)
{
    if (path.empty())
        return nullptr;

    Object *object = nullptr;

    switch (Identify(path)) {
        case kFormatBinary:
            object = BinaryParse(path, error);
            break;

        case kFormatXML:
            object = XMLParse(path, error);
            break;

        case kFormatJSONOpenstep:
            object = JSONParse(path, error);
            break;

        case kFormatSystemError:
            error(0, 0, strerror(errno));
            break;

        default:
            error(0, 0, "not a property list");
            break;
    }

    return object;
}

Object *Object::
Parse(std::string const &path)
{
    return Parse(path, [](unsigned, unsigned, std::string const &) { });
}

Object *Object::
Parse(FILE *fp, error_function const &error)
{
    if (fp == nullptr)
        return nullptr;

    Object *object = nullptr;

    switch (Identify(fp)) {
        case kFormatBinary:
            object = BinaryParse(fp, error);
            break;

        case kFormatXML:
            object = XMLParse(fp, error);
            break;

        case kFormatJSONOpenstep:
            object = JSONParse(fp, error);
            break;

        default:
            error(0, 0, "not a property list");
            break;
    }

    return object;
}

Object *Object::
Parse(FILE *fp)
{
    return Parse(fp, [](unsigned, unsigned, std::string const &) { });
}

//
// Dictionary Parsing
//

Dictionary *Dictionary::
Parse(std::string const &path, error_function const &error)
{
    auto object = Object::Parse(path, error);
    auto dict   = CastTo <Dictionary> (object);
    if (dict == nullptr && object != nullptr) {
        error(0, 0, "not a dictionary property list");
        object->release();
        dict = nullptr;
    }

    return dict;
}

Dictionary *Dictionary::
Parse(std::string const &path)
{
    return Parse(path, [](unsigned, unsigned, std::string const &) { });
}

Dictionary *Dictionary::
Parse(FILE *fp, error_function const &error)
{
    auto object = Object::Parse(fp, error);
    auto dict   = CastTo <Dictionary> (object);
    if (dict == nullptr && object != nullptr) {
        error(0, 0, "not a dictionary property list");
        object->release();
        dict = nullptr;
    }

    return dict;
}

Dictionary *Dictionary::
Parse(FILE *fp)
{
    return Parse(fp, [](unsigned, unsigned, std::string const &) { });
}

Dictionary *Dictionary::
ParseSimpleXML(std::string const &path, error_function const &error)
{
    if (path.empty())
        return nullptr;

    return SimpleXMLParser().parse(path, error);
}

Dictionary *Dictionary::
ParseSimpleXML(std::string const &path)
{
    return ParseSimpleXML(path, [](unsigned, unsigned, std::string const &) { });
}

Dictionary *Dictionary::
ParseSimpleXML(std::FILE *fp, error_function const &error)
{
    if (fp == nullptr)
        return nullptr;

    return SimpleXMLParser().parse(fp, error);
}

Dictionary *Dictionary::
ParseSimpleXML(std::FILE *fp)
{
    return ParseSimpleXML(fp, [](unsigned, unsigned, std::string const &) { });
}

//
// Array Parsing
//

Array *Array::
Parse(std::string const &path, error_function const &error)
{
    auto object = Object::Parse(path, error);
    auto array  = CastTo <Array> (object);
    if (array == nullptr && object != nullptr) {
        error(0, 0, "not an array property list");
        object->release();
        array = nullptr;
    }

    return array;
}

Array *Array::
Parse(std::string const &path)
{
    return Parse(path, [](unsigned, unsigned, std::string const &) { });
}

Array *Array::
Parse(FILE *fp, error_function const &error)
{
    auto object = Object::Parse(fp, error);
    auto array  = CastTo <Array> (object);
    if (array == nullptr && object != nullptr) {
        error(0, 0, "not an array property list");
        object->release();
        array = nullptr;
    }

    return array;
}

Array *Array::
Parse(FILE *fp)
{
    return Parse(fp, [](unsigned, unsigned, std::string const &) { });
}

//
// Utilities
//
using plist::WarnUnhandledKeyMap;

void plist::
WarnUnhandledKeyMap(plist::Dictionary const *dict, char const *name,
        plist::KeyTypeMap const &keys)
{
    if (dict == nullptr)
        return;

    for (size_t n = 0; n < dict->count(); n++) {
        auto I = keys.find(dict->key(n));
        if (I == keys.end()) {
            fprintf(stderr, "warning: %s%s%s"
                    "key '%s' of type '%s' is not handled\n",
                    name != nullptr ? "parsing " : "",
                    name != nullptr ? name : "",
                    name != nullptr ? " " : "",
                    dict->key(n).c_str(),
                    dict->value(n)->typeName());
            continue;
        }

        //
        // If the type used is Object, any type will do!
        //
        if (I->second == Object::Type())
            continue;

        //
        // Handle special boolean case for broken plists that
        // use strings in place of booleans.
        //
        if (I->second == Boolean::Type() &&
                CastTo <Boolean> (dict->value(n)) != nullptr)
            continue;

        if (I->second != dict->value(n)->type()) {
            fprintf(stderr, "warning: %s%s%s"
                    "expecting key '%s' to be of type "
                    "'%s' but it is of type '%s'\n",
                    name != nullptr ? "parsing " : "",
                    name != nullptr ? name : "",
                    name != nullptr ? " " : "",
                    I->first.c_str(),
                    Object::GetTypeName(I->second),
                    dict->value(n)->typeName());
        }
    }
}
