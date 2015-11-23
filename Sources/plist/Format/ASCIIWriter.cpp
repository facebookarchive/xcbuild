/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/ASCIIWriter.h>
#include <plist/Objects.h>

using plist::Format::ASCIIWriter;
using plist::Object;
using plist::String;
using plist::Integer;
using plist::Real;
using plist::Boolean;
using plist::Data;
using plist::Array;
using plist::Dictionary;
using plist::CastTo;

ASCIIWriter::
ASCIIWriter(Object const *root) :
    _root   (root),
    _indent (0),
    _lastKey(false)
{
}

ASCIIWriter::
~ASCIIWriter()
{
}

static bool
NeedsQuoting(std::string const &string)
{
    for (char c : string) {
        if (!isalnum(c) && c != '_') {
            return true;
        }
    }

    return false;
}

bool ASCIIWriter::
write()
{
    if (!handleObject(_root)) {
        return false;
    }

    if (!writeString("\n", false)) {
        return false;
    }

    return true;
}

/*
 * Low level functions.
 */

bool ASCIIWriter::
primitiveWriteString(std::string const &string)
{
    _contents.insert(_contents.end(), string.begin(), string.end());
    return true;
}

bool ASCIIWriter::
primitiveWriteEscapedString(std::string const &string)
{
    _contents.reserve(_contents.size() + string.size());

    if (!primitiveWriteString("\"")) {
        return false;
    }

    for (std::string::const_iterator it = string.begin(); it != string.end(); ++it) {
        uint8_t c = *it;

        if (c > 0x80) {
            uint32_t codepoint;

            if ((c & 0xE0) == 0xC0) {
                uint8_t next = *(++it);
                codepoint = ((c & 0x1F) << 6) | next;
            } else if ((c & 0xF0) == 0xE0) {
                uint8_t next1 = *(++it);
                uint8_t next2 = *(++it);
                codepoint = ((c & 0x0F) << 12) | (next1 << 6) | next2;
            } else if ((c & 0xF8) == 0xF0) {
                uint8_t next1 = *(++it);
                uint8_t next2 = *(++it);
                uint8_t next3 = *(++it);
                codepoint = (((c & 0x0F) << 18) | (next1 << 12) | (next2 << 6) | next3) + 65536;
            } else {
                /* Invalid UTF-8. */
                return false;
            }

            if (codepoint < 0xFF) {
                char buf[64];
                int rc = snprintf(buf, sizeof(buf), "\\%03o", codepoint);
                assert(rc < (int)sizeof(buf));

                if (!primitiveWriteString(buf)) {
                    return false;
                }
            } else {
                char buf[64];
                int rc = snprintf(buf, sizeof(buf), "\\%04u", codepoint);
                assert(rc < (int)sizeof(buf));

                if (!primitiveWriteString(buf)) {
                    return false;
                }
            }
        } else {
            switch (c) {
                case '\a': if (!primitiveWriteString("\\a"))  { return false; } break;
                case '\b': if (!primitiveWriteString("\\b"))  { return false; } break;
                case '\v': if (!primitiveWriteString("\\v"))  { return false; } break;
                case '\f': if (!primitiveWriteString("\\f"))  { return false; } break;
                case '"':  if (!primitiveWriteString("\\\"")) { return false; } break;
                case '\\': if (!primitiveWriteString("\\"))   { return false; } break;
                default: _contents.push_back(c); break;
            }
        }
    }

    if (!primitiveWriteString("\"")) {
        return false;
    }

    return true;
}

bool ASCIIWriter::
writeString(std::string const &string, bool final)
{
    if (final) {
        for (int n = 0; n < _indent; n++) {
            if (!primitiveWriteString("\t")) {
                return false;
            }
        }
    }

    return primitiveWriteString(string);
}

bool ASCIIWriter::
writeEscapedString(std::string const &string, bool final)
{
    if (!NeedsQuoting(string)) {
        return writeString(string, final);
    }

    if (final) {
        for (int n = 0; n < _indent; n++) {
            if (!primitiveWriteString("\t")) {
                return false;
            }
        }
    }

    return primitiveWriteEscapedString(string);
}

/*
 * Higher level functions.
 */

bool ASCIIWriter::
handleObject(Object const *object)
{
    if (Dictionary const *dictionary = CastTo<Dictionary>(object)) {
        if (!handleDictionary(dictionary)) {
            return false;
        }
    } else if (Array const *array = CastTo<Array>(object)) {
        if (!handleArray(array)) {
            return false;
        }
    } else if (Boolean const *boolean = CastTo<Boolean>(object)) {
        if (!handleBoolean(boolean)) {
            return false;
        }
    } else if (Integer const *integer = CastTo<Integer>(object)) {
        if (!handleInteger(integer)) {
            return false;
        }
    } else if (Real const *real = CastTo<Real>(object)) {
        if (!handleReal(real)) {
            return false;
        }
    } else if (String const *string = CastTo<String>(object)) {
        if (!handleString(string)) {
            return false;
        }
    } else if (Data const *data = CastTo<Data>(object)) {
        if (!handleData(data)) {
            return false;
        }
    } else if (Date const *date = CastTo<Date>(object)) {
        if (!handleDate(date)) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool ASCIIWriter::
handleDictionary(Dictionary const *dictionary)
{
    /* Write '{'. */
    if (!writeString("{\n", !_lastKey)) {
        return false;
    }

    _lastKey = false;

    _indent++;

    for (int i = 0; i < dictionary->count(); ++i) {
        _lastKey = false;

        if (!writeEscapedString(dictionary->key(i), !_lastKey)) {
            return false;
        }

        if (!writeString(" = ", false)) {
            return false;
        }
        
        _lastKey = true;

        if (!handleObject(dictionary->value(i))) {
            return false;
        }

        if (!writeString(";\n", false)) {
            return false;
        }
    }

    _indent--;
    return writeString("}", true);
}

bool ASCIIWriter::
handleArray(Array const *array)
{
    /* Write '('. */
    if (!writeString("(\n", !_lastKey)) {
        return false;
    }

    _lastKey = false;

    _indent++;

    for (int i = 0; i < array->count(); ++i) {
        /* Write ',' if not first entry. */
        if (i != 0) {
            if (!writeString(",\n", false)) {
                return false;
            }
        }

        if (!handleObject(array->value(i))) {
            return false;
        }
    }

    /* Write ')'. */
    if (!writeString("\n", false)) {
        return false;
    }

    _indent--;
    return writeString(")", true);
}

bool ASCIIWriter::
handleBoolean(Boolean const *boolean)
{
    if (!writeString(boolean->value() ? "YES" : "NO", !_lastKey)) {
        return false;
    }

    _lastKey = false;
    return true;
}

bool ASCIIWriter::
handleString(String const *string)
{
    if (!writeEscapedString(string->value(), !_lastKey)) {
        return false;
    }

    _lastKey = false;
    return true;
}

bool ASCIIWriter::
handleData(Data const *data)
{
    if (!writeString("<", !_lastKey)) {
        return false;
    }

    _lastKey = false;

    std::vector<uint8_t> const &value = data->value();
    for (int i = 0; i < value.size(); ++i) {
        char buf[3];
        int  rc = snprintf(buf, sizeof(buf), "%02x", value[i]);
        assert(rc < (int)sizeof(buf));

        if (!writeString(buf, false)) {
            return false;
        }
    }

    return writeString(">", false);
}

bool ASCIIWriter::
handleReal(Real const *real)
{
    char buf[64];
    int rc = snprintf(buf, sizeof(buf), "%.17g", real->value());
    assert(rc < (int)sizeof(buf));

    if (!writeString(buf, !_lastKey)) {
        return false;
    }

    _lastKey = false;

    return true;
}

bool ASCIIWriter::
handleInteger(Integer const *integer)
{
    int               rc;
    char              buf[32];

    rc = snprintf(buf, sizeof(buf), "%lld", integer->value());
    assert(rc < (int)sizeof(buf));

    if (!writeString(buf, !_lastKey)) {
        return false;
    }

    _lastKey = false;

    return true;
}

bool ASCIIWriter::
handleDate(Date const *date)
{
    if (!writeEscapedString(date->stringValue(), !_lastKey)) {
        return false;
    }

    _lastKey = false;
    return true;
}

