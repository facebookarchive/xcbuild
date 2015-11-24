/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_ABPWriterPrivate_h
#define __plist_Format_ABPWriterPrivate_h

#include <plist/Format/ABPCoderPrivate.h>

/* Raw Data Write Helpers */

static bool
__ABPWriteByte(ABPContext *context, uint8_t byte)
{
    return (__ABPWriteBytes(context, &byte, sizeof(byte)) == sizeof(byte));
}

static bool
__ABPWriteWord0(ABPContext *context, size_t nbytes, uint64_t value,
        bool swap)
{
    size_t   n;
    uint8_t *p, bytes[8];

    assert(nbytes <= 8);
    memset(bytes, 0, sizeof(bytes));
    p = bytes;

    for (n = 0; n < nbytes; n++) {
        size_t m = swap ? n : (nbytes - n - 1);
        *p++ = value >> (m << 3);
    }

    return (__ABPWriteBytes(context, bytes, nbytes) == nbytes);
}

static bool
__ABPWriteWord(ABPContext *context, size_t nbytes, uint64_t value)
{
    return __ABPWriteWord0(context, nbytes, value, false);
}

static bool
__ABPWriteTypeAndLength(ABPContext *context, ABPRecordType type, uint64_t length)
{
    uint8_t marker       = 0;
    uint8_t directLength = 0;
    int     nbits        = 3;

    assert(length >= 0);
    if (length >= 0x0f) {
        /* Prepare marker byte. */
        if ((length & 0xFF) == length) {
            nbits = 0;
        } else if ((length & 0xFFFF) == length) {
            nbits = 1;
        } else if ((length & 0xFFFFFFFF) == length) {
            nbits = 2;
        }

        marker = 0x10 | nbits;
        directLength = 0xf;
    } else {
        directLength = length;
    }

    /* Write the object type and direct length. */
    if (!__ABPWriteByte(context, __ABPRecordTypeToByte(type, directLength)))
        return false;

    /* Write the marker and complete length if needed. */
    if (marker != 0) {
        if (!__ABPWriteByte(context, marker))
            return false;

        if (!__ABPWriteWord(context, 1 << nbits, length))
            return false;
    } 
            
    return true;
}

static bool
__ABPWriteOffset(ABPContext *context, uint64_t offset)
{
    return __ABPWriteWord(context, context->trailer.offsetIntByteSize, offset);
}

static bool
__ABPWriteReference(ABPContext *context, uint64_t offset)
{
    return __ABPWriteWord(context, context->trailer.objectRefByteSize, offset);
}

static bool
__ABPWriteHeader(ABPContext *context)
{
    if (__ABPSeek(context, 0, SEEK_SET) == EOF)
        return false;

    return (__ABPWriteBytes(context, &context->header,
                sizeof(context->header)) == sizeof(context->header));
}

static bool
__ABPWriteTrailer(ABPContext *context, bool replace)
{
    if (__ABPSeek(context, replace ? -sizeof(context->trailer) : 0, SEEK_END) == EOF)
        return false;

    if (__ABPWriteBytes(context, context->trailer.__filler, 8) != 8)
        return false;
    if (!__ABPWriteWord(context, 8, context->trailer.objectsCount))
        return false;
    if (!__ABPWriteWord(context, 8, context->trailer.topLevelObject))
        return false;
    if (!__ABPWriteWord(context, 8, context->trailer.offsetTableEndOffset))
        return false;

    return true;
}

#endif  /* !__plist_Format_ABPWriterPrivate_h */
