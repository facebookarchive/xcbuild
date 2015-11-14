/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/SimpleXMLParser.h>
#include <plist/XMLParser.h>
#include <plist/BinaryParser.h>
#include <plist/ASCIIParser.h>
#include <plist/StringsParser.h>

#include <plist/Objects.h>
#include <plist/Keys.h>

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

namespace {

static Object *
ASCIIParse(std::FILE *fp, error_function const &error)
{
    return ASCIIParser().parse(fp, error);
}

static Object *
ASCIIParse(std::string const &path, error_function const &error)
{
    return ASCIIParser().parse(path, error);
}

static Object *
StringsParse(std::FILE *fp, error_function const &error)
{
    return StringsParser().parse(fp, error);
}

static Object *
StringsParse(std::string const &path, error_function const &error)
{
    return StringsParser().parse(path, error);
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
    kFormatASCII,
    kFormatStrings,
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
                    format = kFormatASCII;
                    break;
                } else if (buf[n] == '"' || buf[n] == '\'') {
                    format = kFormatStrings;
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

        case kFormatASCII:
            object = ASCIIParse(path, error);
            break;

        case kFormatStrings:
            object = StringsParse(path, error);
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

        case kFormatStrings:
            object = StringsParse(fp, error);
            break;

        case kFormatASCII:
            object = ASCIIParse(fp, error);
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
