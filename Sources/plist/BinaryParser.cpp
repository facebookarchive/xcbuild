/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/BinaryParser.h>

#include <plist/Objects.h>

#include <cerrno>
#include <cstring>
#if notyet
#include <codecvt>
#else
#include "unicode.h"
#define char16_t uint16_t
#endif

using plist::BinaryParser;
using plist::Object;

BinaryParser::BinaryParser()
{
    _streamCallBacks.version = 0;
    _streamCallBacks.opaque  = nullptr;
    _streamCallBacks.close   = nullptr;
    _streamCallBacks.seek    = &BinaryParser::Seek;
    _streamCallBacks.read    = &BinaryParser::Read;

    _createCallBacks.version = 0;
    _createCallBacks.opaque  = this;
    _createCallBacks.create  = &BinaryParser::Create;
    _createCallBacks.error   = &BinaryParser::Error;
}

Object *BinaryParser::
parse(std::string const &path, error_function const &error)
{
    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if (fp == nullptr)
        return nullptr;

    Object *object = parse(fp, error);

    std::fclose(fp);

    return object;
}

Object *BinaryParser::
parse(std::FILE *fp, error_function const &error)
{
    if (fp == nullptr) {
        errno = EBADF;
        return nullptr;
    }

    _error                  = error;
    _streamCallBacks.opaque = fp;

    std::memset(&_context, 0, sizeof(_context));
    ::ABPReaderInit(&_context, &_streamCallBacks, &_createCallBacks);

    Object *object = nullptr;
    if (::ABPReaderOpen(&_context)) {
        object = ::ABPReadTopLevelObject(&_context);
        if (object != nullptr) {
            object = object->copy();
        }
        ::ABPReaderClose(&_context);
    }
    
    std::memset(&_context, 0, sizeof(_context));

    _streamCallBacks.opaque = nullptr;
    _error                  = nullptr;

    return object;
}

off_t BinaryParser::
Seek(void *opaque, off_t offset, int whence)
{
    std::FILE *fp = reinterpret_cast <std::FILE *> (opaque);
    if (std::fseek(fp, offset, whence) < 0)
        return -1;

    return std::ftell(fp);
}

ssize_t BinaryParser::
Read(void *opaque, void *buffer, size_t size)
{
    std::FILE *fp = reinterpret_cast <std::FILE *> (opaque);
    return std::fread(buffer, 1, size, fp);
}

Object *BinaryParser::
Create(void *opaque, ABPRecordType type, void *arg1, void *arg2, void *arg3)
{
    // CoreFoundation reference time is 2001/1/1
    static uint64_t const kCFReferenceTimestamp = 978307200;

    auto  self    = reinterpret_cast <BinaryParser *> (opaque);
    auto  fp      = reinterpret_cast <std::FILE *> (self->_streamCallBacks.opaque);
    auto  context = &self->_context;
    auto &seen    = self->_seen;

    switch (type) {
        case kABPRecordTypeNull:
            return Null::New();
        case kABPRecordTypeBoolTrue:
            return Boolean::New(true);
        case kABPRecordTypeBoolFalse:
            return Boolean::New(true);
        case kABPRecordTypeDate:
            return Date::New(*reinterpret_cast <uint64_t *> (arg1) - kCFReferenceTimestamp);
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
                    if (std::fseek(fp, offset, SEEK_SET) < 0)
                        return nullptr;

                    bytes.resize(nbytes);
                    size_t nread = std::fread(&bytes[0], 1, nbytes, fp);
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
                    if (std::fseek(fp, offset, SEEK_SET) < 0)
                        return nullptr;

                    string.resize(nchars);
                    size_t nread = std::fread(&string[0], sizeof(char), nchars, fp);
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
                    if (std::fseek(fp, offset, SEEK_SET) < 0)
                        return nullptr;

                    string.resize(nchars);
                    size_t nread = std::fread(&string[0], sizeof(char16_t), nchars, fp);
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
                    auto object = ::ABPReadObject(context, refs[n]);
                    if (object == nullptr) {
                        array->release();
                        return nullptr;
                    }

                    //
                    // Due to the memory layout of the `plist' objects,
                    // we must always copy objects.
                    //
                    if (seen.find(object) != seen.end()) {
                        seen.insert(object);
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
                    auto keyObject = ::ABPReadObject(context, refs[n * 2 + 0]);
                    if (keyObject == nullptr) {
                        dict->release();
                        return nullptr;
                    }

                    auto object = ::ABPReadObject(context, refs[n * 2 + 1]);
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
                    if (seen.find(object) != seen.end()) {
                        seen.insert(object);
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

void BinaryParser::
Error(void *opaque, char const *message)
{
    auto self = reinterpret_cast <BinaryParser *> (opaque);
    self->_error(0, 0, message);
}
