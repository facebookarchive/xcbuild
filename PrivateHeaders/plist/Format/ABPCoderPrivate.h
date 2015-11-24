/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_ABPCoderPrivate_h
#define __plist_Format_ABPCoderPrivate_h

#include <plist/Format/ABPCoder.h>

#include <plist/Objects.h>

#include <stdarg.h>
#include <stdio.h>

static inline ABPRecordType
__ABPByteToRecordType(uint8_t byte)
{
    if ((byte & 0xf0) == 0 || byte == 0x33) {
        switch (byte) {
            case 0x00: /* 0000 0000: null */
                return kABPRecordTypeNull;
            case 0x08: /* 0000 1000: false */
                return kABPRecordTypeBoolFalse;
            case 0x09: /* 0000 1001: true */
                return kABPRecordTypeBoolTrue;
            case 0x0f: /* 0000 1111: fill byte */
                return kABPRecordTypeFill;
            case 0x33: /* 0011 0011: date (double be) */
                return kABPRecordTypeDate;
            default:
                return kABPRecordTypeInvalid;
        }
    } else {
        switch (byte >> 4) {
            case 0x1: /* 0001 nnnn: int */
                return kABPRecordTypeInteger;
            case 0x2: /* 0010 nnnn: real */
                return kABPRecordTypeReal;
            case 0x4: /* 0100 nnnn: data */
                return kABPRecordTypeData;
            case 0x5: /* 0101 nnnn: ascii string */
                return kABPRecordTypeStringASCII;
            case 0x6: /* 0110 nnnn: unicode string */
                return kABPRecordTypeStringUnicode;
            case 0xa: /* 1010 nnnn: array */
                return kABPRecordTypeArray;
            case 0xd: /* 1101 nnnn: dictionary */
                return kABPRecordTypeDictionary;
            default:
                return kABPRecordTypeInvalid;
        }
    }
}

static inline int
__ABPRecordTypeToByte(ABPRecordType type, size_t size)
{
    switch (type) {
        case kABPRecordTypeNull:
            return 0x00; /* 0000 0000: null */
        case kABPRecordTypeBoolFalse:
            return 0x08; /* 0000 1000: false */
        case kABPRecordTypeBoolTrue:
            return 0x09; /* 0000 1001: true */
        case kABPRecordTypeFill:
            return 0x0f; /* 0000 1111: fill byte */
        case kABPRecordTypeDate:
            return 0x33; /* 0011 0011: date (double be) */
        case kABPRecordTypeInteger:
            return 0x10 | (size & 0xf); /* 0001 nnnn: int */
        case kABPRecordTypeReal:
            return 0x20 | (size & 0xf); /* 0010 nnnn: real */
        case kABPRecordTypeData:
            if (size > 15) size = 0xf;
            return 0x40 | (size & 0xf); /* 0100 nnnn: data */
        case kABPRecordTypeStringASCII:
            if (size > 15) size = 0xf;
            return 0x50 | (size & 0xf); /* 0101 nnnn: ascii string */
        case kABPRecordTypeStringUnicode:
            if (size > 15) size = 0xf;
            return 0x60 | (size & 0xf); /* 0110 nnnn: unicode string */
        case kABPRecordTypeArray:
            if (size > 15) size = 0xf;
            return 0xa0 | (size & 0xf); /* 1010 nnnn: array */
        case kABPRecordTypeDictionary:
            if (size > 15) size = 0xf;
            return 0xd0 | (size & 0xf); /* 1101 nnnn: dictionary */
        default:
            return EOF;
    }
}

/* Stream CallBacks */

static inline off_t
__ABPSeek(ABPContext *context, off_t offset, int whence)
{
    if (context->streamCallBacks.seek == NULL)
        return EOF;
    else
        return (*(context->streamCallBacks.seek))(context->streamCallBacks.opaque,
                offset, whence);
}

static inline off_t
__ABPTell(ABPContext *context)
{
    return __ABPSeek(context, 0, SEEK_CUR);
}

static inline ssize_t
__ABPReadBytes(ABPContext *context, void *data, size_t length)
{
    if (context->streamCallBacks.read == NULL)
        return EOF;
    else
        return (*(context->streamCallBacks.read))(context->streamCallBacks.opaque,
                data, length);
}

static inline ssize_t
__ABPWriteBytes(ABPContext *context, void const *data, size_t length)
{
    if (context->streamCallBacks.write == NULL)
        return EOF;
    else
        return (*(context->streamCallBacks.write))(context->streamCallBacks.opaque,
                                                   data, length);
}

/* Reader Objects Callback */

static inline void
__ABPError(ABPContext *context, char const *format, ...)
{
    static char const sErrorMessage[] = "file corrupted";
    va_list  ap;
    char    *buf;

    if (context->createCallBacks.error != NULL) {
        va_start(ap, format);
        if (vasprintf(&buf, format, ap) < 0) {
            buf = (char *)sErrorMessage;
        }
        va_end(ap);

        (*(context->createCallBacks.error))(context->createCallBacks.opaque, buf);

        if (buf != sErrorMessage) {
            free(buf);
        }
    }
}

static inline plist::Object *
__ABPCreateObject(ABPContext *context, ABPRecordType type, void *arg0 = NULL,
        void *arg1 = NULL, void *arg2 = NULL)
{
    if (context->createCallBacks.create == NULL)
        return NULL;
    else
        return (*(context->createCallBacks.create))(context->createCallBacks.opaque,
                type, arg0, arg1, arg2);
}

static inline plist::Null *
__ABPCreateNull(ABPContext *context)
{
    return plist::CastTo <plist::Null> (__ABPCreateObject(context, kABPRecordTypeNull));
}

static inline plist::Boolean *
__ABPCreateBool(ABPContext *context, bool value)
{
    return plist::CastTo <plist::Boolean> (
            __ABPCreateObject(context, value ? kABPRecordTypeBoolTrue :
                kABPRecordTypeBoolFalse));
}

static inline plist::Integer *
__ABPCreateInteger(ABPContext *context, int64_t value, size_t nbytes)
{
    return plist::CastTo <plist::Integer> (__ABPCreateObject(context,
                kABPRecordTypeInteger, &value, &nbytes));
}

static inline plist::Real *
__ABPCreateReal(ABPContext *context, int64_t value, size_t nbytes)
{
    return plist::CastTo <plist::Real> (__ABPCreateObject(context,
                kABPRecordTypeReal, &value, &nbytes));
}

static inline plist::String *
__ABPCreateString(ABPContext *context, off_t offset, size_t nchars,
        bool unicode)
{
    return plist::CastTo <plist::String> (__ABPCreateObject(context,
            unicode ? kABPRecordTypeStringUnicode :
            kABPRecordTypeStringASCII, &offset, &nchars));
}

static inline plist::Data *
__ABPCreateData(ABPContext *context, off_t offset, size_t nbytes)
{
    return plist::CastTo <plist::Data> (__ABPCreateObject(context,
                kABPRecordTypeData, &offset, &nbytes));
}

static inline plist::Date *
__ABPCreateDate(ABPContext *context, uint64_t timestamp)
{
    return plist::CastTo <plist::Date> (__ABPCreateObject(context,
                kABPRecordTypeDate, &timestamp));
}

static inline plist::Array *
__ABPCreateArray(ABPContext *context, uint64_t *refs, size_t nrefs)
{
    return plist::CastTo <plist::Array> (__ABPCreateObject(context,
                kABPRecordTypeArray, refs, &nrefs));
}

static inline plist::Dictionary *
__ABPCreateDictionary(ABPContext *context, uint64_t *refs, size_t nrefs)
{
    return plist::CastTo <plist::Dictionary> (__ABPCreateObject(context,
                kABPRecordTypeDictionary, refs, &nrefs));
}

void
_ABPContextFree(ABPContext *context);

#endif  /* !__plist_Format_ABPCoderPrivate_h */
