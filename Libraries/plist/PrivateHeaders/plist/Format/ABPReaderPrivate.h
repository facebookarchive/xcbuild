/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_ABPReaderPrivate_h
#define __plist_Format_ABPReaderPrivate_h

#include <plist/Format/ABPCoderPrivate.h>

#include <cstring>

/* Raw Data Read Helpers */

static inline int
__ABPReadByte(ABPContext *context)
{
    uint8_t byte;

    if (__ABPReadBytes(context, &byte, sizeof(byte)) != sizeof(byte))
        return EOF;

    return byte;
}

static inline bool
__ABPReadWord(ABPContext *context, size_t nbytes, uint64_t *result)
{
    uint8_t  *p, bytes[8];
    uint64_t  v;

    if (__ABPReadBytes(context, bytes, nbytes) != nbytes)
        return false;

    p = bytes, v = 0;
    switch (nbytes) {
        default: return false;
        case 8: v |= (uint64_t)*p++ << 56;
        case 7: v |= (uint64_t)*p++ << 48;
        case 6: v |= (uint64_t)*p++ << 40;
        case 5: v |= (uint64_t)*p++ << 32;
        case 4: v |= (uint64_t)*p++ << 24;
        case 3: v |= (uint64_t)*p++ << 16;
        case 2: v |= (uint64_t)*p++ << 8;
        case 1: v |= (uint64_t)*p++;
        case 0: break;
    }

    *result = v;
    return true;
}

static inline bool
__ABPReadLength(ABPContext *context, size_t *nitems)
{
    if (*nitems == 0x0f) {
        int marker = __ABPReadByte(context);
        if (marker == EOF)
            return false;

        if ((marker & 0xf0) == 0x10) {
            uint64_t value;

            if (!__ABPReadWord(context, 1 << (marker & 0x0f), &value))
                return false;

            *nitems = value;
        } else {
            return false;
        }
    }

    return true;
}

static inline bool
__ABPReadOffset(ABPContext *context, uint64_t *offset)
{
    return __ABPReadWord(context, context->trailer.offsetIntByteSize, offset);
}

static inline bool
__ABPReadReference(ABPContext *context, uint64_t *offset)
{
    return __ABPReadWord(context, context->trailer.objectRefByteSize, offset);
}

static inline bool
__ABPReadHeader(ABPContext *context)
{
    if (__ABPSeek(context, 0, SEEK_SET) == EOF)
        return false;

    if (__ABPReadBytes(context, &context->header,
                sizeof(context->header)) != sizeof(context->header))
        return false;

    return (memcmp(context->header.magic, ABPLIST_MAGIC,
                sizeof(context->header.magic)) == 0 &&
            memcmp(context->header.version, ABPLIST_VERSION,
                sizeof(context->header.version)) == 0);
}

static inline bool
__ABPReadTrailer(ABPContext *context)
{
    if (__ABPSeek(context, -sizeof(context->trailer), SEEK_END) == EOF)
        return false;

    if (__ABPReadBytes(context, context->trailer.__filler, 8) != 8)
        return false;
    if (!__ABPReadWord(context, 8, &context->trailer.objectsCount))
        return false;
    if (!__ABPReadWord(context, 8, &context->trailer.topLevelObject))
        return false;
    if (!__ABPReadWord(context, 8, &context->trailer.offsetTableEndOffset))
        return false;

    return true;
}

#endif  /* !__plist_Format_ABPReaderPrivate_h */
