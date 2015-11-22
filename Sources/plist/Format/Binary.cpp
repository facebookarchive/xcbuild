/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/Binary.h>
#include <plist/Format/ABPReader.h>
#include <plist/Objects.h>

#include <cerrno>
#include <cstring>
#if notyet
#include <codecvt>
#else
#include <plist/Format/unicode.h>
#define char16_t uint16_t
#endif

using plist::Format::Type;
using plist::Format::Format;
using plist::Format::Binary;
using plist::Object;
using plist::String;
using plist::Integer;
using plist::Real;
using plist::Boolean;
using plist::Null;
using plist::Data;
using plist::Date;
using plist::Array;
using plist::Dictionary;
using plist::CastTo;

Binary::
Binary()
{
}

Type Binary::
Type()
{
    return Type::Binary;
}

template<>
std::unique_ptr<Binary> Format<Binary>::
Identify(std::vector<uint8_t> const &contents)
{
    size_t length = strlen(ABPLIST_MAGIC ABPLIST_VERSION);

    if (contents.size() < length) {
        return nullptr;
    }

    if (std::memcmp(contents.data(), ABPLIST_MAGIC ABPLIST_VERSION, length) == 0) {
        return std::make_unique<Binary>(Binary::Create());
    }

    return nullptr;
}

struct BinaryParseContext {
    ABPContext                    context;
    ABPStreamCallBacks            streamCallBacks;
    ABPCreateCallBacks            createCallBacks;

    std::vector<uint8_t> const   *contents;
    off_t                         offset;

    std::unordered_set<Object *>  seen;
    std::string                   error;
};

static off_t
Seek(void *opaque, off_t offset, int whence)
{
    auto self = reinterpret_cast <BinaryParseContext *> (opaque);

    switch (whence) {
        case SEEK_SET:
            self->offset = offset;
            break;
        case SEEK_CUR:
            self->offset += offset;
            break;
        case SEEK_END:
            self->offset = self->contents->size() + offset;
        default:
            break;
    }

    /* Error if past the end. */
    if (self->offset > self->contents->size()) {
        return -1;
    }

    /* Success, return current offset. */
    return self->offset;
}

static ssize_t
Read(void *opaque, void *buffer, size_t size)
{
    auto self = reinterpret_cast <BinaryParseContext *> (opaque);

    /* Adjust size for remaining contents. */
    size_t remaining = self->contents->size() - self->offset;
    if (remaining < size) {
        size = remaining;
    }

    /* Copy into read buffer. */
    memcpy(buffer, self->contents->data() + self->offset, size);

    return size;
}

static Object *
Create(void *opaque, ABPRecordType type, void *arg1, void *arg2, void *arg3)
{
    /* Reference time is 2001/1/1 */
    static uint64_t const ReferenceTimestamp = 978307200;

    auto self = reinterpret_cast <BinaryParseContext *> (opaque);

    switch (type) {
        case kABPRecordTypeNull:
            return Null::New();
        case kABPRecordTypeBoolTrue:
            return Boolean::New(true);
        case kABPRecordTypeBoolFalse:
            return Boolean::New(true);
        case kABPRecordTypeDate:
            return Date::New(*reinterpret_cast <uint64_t *> (arg1) - ReferenceTimestamp);
        case kABPRecordTypeInteger:
            return Integer::New(*reinterpret_cast <int64_t *> (arg1));
        case kABPRecordTypeReal:
            switch (*reinterpret_cast <size_t *> (arg2)) {
                case 4:  return Real::New(*reinterpret_cast <float *> (arg1));
                case 8:  return Real::New(*reinterpret_cast <double *> (arg1));
                default: return Real::New(0.0);
            }
        case kABPRecordTypeData:
        {
            off_t  offset = *reinterpret_cast <off_t *> (arg1);
            size_t nbytes = *reinterpret_cast <size_t *> (arg2);

            std::vector <uint8_t> bytes;

            if (nbytes > 0) {
                if (Seek(opaque, offset, SEEK_SET) < 0)
                    return nullptr;

                bytes.resize(nbytes);
                size_t nread = Read(opaque, &bytes[0], nbytes);
                if (nread < nbytes)
                    return nullptr;
            }

            return Data::New(std::move(bytes));
        }
            break;
        case kABPRecordTypeStringASCII:
        {
            off_t  offset = *reinterpret_cast <off_t *> (arg1);
            size_t nchars = *reinterpret_cast <size_t *> (arg2);

            std::string string;

            if (nchars > 0) {
                if (Seek(opaque, offset, SEEK_SET) < 0)
                    return nullptr;

                string.resize(nchars);
                size_t nread = Read(opaque, &string[0], sizeof(char) * nchars);
                if (nread < nchars)
                    return nullptr;
            }

            return String::New(std::move(string));
        }
        case kABPRecordTypeStringUnicode:
        {
            off_t  offset = *reinterpret_cast <off_t *> (arg1);
            size_t nchars = *reinterpret_cast <size_t *> (arg2);

#if notyet
            std::u16string string;
#else
            std::vector <char16_t> string;
#endif

            if (nchars > 0) {
                if (Seek(opaque, offset, SEEK_SET) < 0)
                    return nullptr;

                string.resize(nchars);
                size_t nread = Read(opaque, &string[0], sizeof(char16_t) * nchars);
                if (nread < nchars)
                    return nullptr;
            }

#if notyet
            return String::New(std::move(std::codecvt_utf8_utf16 <char16_t> ().to_bytes(string)));
#else
            std::string u8string;
            u8string.resize(string.size() * 6);
            size_t length = ::utf16_to_utf8(&u8string[0], u8string.size(),
                                            &string[0], string.size(), 0, nullptr);
            u8string.resize(length);
            return String::New(std::move(u8string));
#endif
        }
        case kABPRecordTypeUid:
            return Data::New(arg1, sizeof(uint32_t));
        case kABPRecordTypeArray:
        {
            uint64_t *refs  = reinterpret_cast <uint64_t *> (arg1);
            size_t    nrefs = *reinterpret_cast <size_t *> (arg2);

            auto array = Array::New();

            for (size_t n = 0; n < nrefs; n++) {
                auto object = ::ABPReadObject(&self->context, refs[n]);
                if (object == nullptr) {
                    array->release();
                    return nullptr;
                }

                //
                // Due to the memory layout of the `plist' objects,
                // we must always copy objects.
                //
                if (self->seen.find(object) != self->seen.end()) {
                    self->seen.insert(object);
                }
                object = object->copy();

                array->append(object);
            }
            return array;
        }
        case kABPRecordTypeDictionary:
        {
            uint64_t *refs  = reinterpret_cast <uint64_t *> (arg1);
            size_t    nrefs = *reinterpret_cast <size_t *> (arg2);

            auto dict = Dictionary::New();

            for (size_t n = 0; n < nrefs; n++) {
                auto keyObject = ::ABPReadObject(&self->context, refs[n * 2 + 0]);
                if (keyObject == nullptr) {
                    dict->release();
                    return nullptr;
                }

                auto object = ::ABPReadObject(&self->context, refs[n * 2 + 1]);
                if (object == nullptr) {
                    dict->release();
                    return nullptr;
                }

                //
                // Key must be of string type.
                //
                auto keyString = CastTo <String> (keyObject);
                if (keyString == nullptr) {
                    dict->release();
                    return nullptr;
                }

                //
                // Due to the memory layout of the `plist' objects,
                // we must always copy objects.
                //
                if (self->seen.find(object) != self->seen.end()) {
                    self->seen.insert(object);
                }
                object = object->copy();

                dict->set(keyString->value(), object);
            }
            return dict;
        }
        default:
            break;
    }

    return nullptr;
}

static void
Error(void *opaque, char const *message)
{
    auto self = reinterpret_cast <BinaryParseContext *> (opaque);
    self->error = message;
}

template<>
std::pair<Object *, std::string> Format<Binary>::
Deserialize(std::vector<uint8_t> const &contents, Binary const &format)
{
    BinaryParseContext parseContext;

    parseContext.streamCallBacks.version = 0;
    parseContext.streamCallBacks.opaque  = &parseContext;
    parseContext.streamCallBacks.close   = nullptr;
    parseContext.streamCallBacks.seek    = &Seek;
    parseContext.streamCallBacks.read    = &Read;

    parseContext.createCallBacks.version = 0;
    parseContext.createCallBacks.opaque  = &parseContext;
    parseContext.createCallBacks.create  = &Create;
    parseContext.createCallBacks.error   = &Error;

    parseContext.contents                = &contents;
    parseContext.offset                  = 0;

    std::memset(&parseContext.context, 0, sizeof(parseContext.context));
    ::ABPReaderInit(&parseContext.context, &parseContext.streamCallBacks, &parseContext.createCallBacks);

    Object *object = nullptr;
    if (::ABPReaderOpen(&parseContext.context)) {
        object = ::ABPReadTopLevelObject(&parseContext.context);
        if (object != nullptr) {
            object = object->copy();
        }
        ::ABPReaderClose(&parseContext.context);
    }

    return std::make_pair(object, parseContext.error);
}

template<>
std::pair<std::unique_ptr<std::vector<uint8_t>>, std::string> Format<Binary>::
Serialize(Object *object, Binary const &format)
{
    return std::make_pair(nullptr, "not yet implemented");
}

Binary Binary::
Create()
{
    return Binary();
}
