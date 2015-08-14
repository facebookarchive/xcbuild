/* Copyright (c) 2013-present Facebook. All rights reserved. */

#include <plist/ABPReaderPrivate.h>

#if 0
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...)
#endif

static bool
_ABPReadOffsetTable(ABPContext *context)
{
    off_t        offset;
    uint64_t     n, *offsets;
    plist::Object **objects;

    offset = -(sizeof(context->trailer) + context->trailer.offsetIntByteSize
            * context->trailer.objectsCount);

    if (__ABPSeek(context, offset, SEEK_END) < 0)
        return false;

    offsets = new uint64_t[context->trailer.objectsCount];
    objects = new plist::Object *[context->trailer.objectsCount];

    for (n = 0; n < context->trailer.objectsCount; n++) {
        objects[n] = nullptr;
        if (!__ABPReadOffset(context, offsets + n)) {
            delete[] objects;
            delete[] offsets;
            return false;
        }

#if 1
        dprintf("Object #%-5llu: %llx%s\n",
                (unsigned long long)n,
                (unsigned long long)offsets[n],
                (n == context->trailer.topLevelObject)
                ? " [TOP LEVEL OBJECT]" : "");
#endif
    }

    context->offsets = offsets;
    context->objects = objects;

    return true;
}

static inline plist::Date *
__ABPReadDate(ABPContext *context)
{
    uint64_t date;
    if (!__ABPReadWord(context, 8, &date)) {
        __ABPError(context, "EOF reading date value");
        return NULL;
    }

    return __ABPCreateDate(context, date);
}

static inline plist::Integer *
__ABPReadInteger(ABPContext *context, size_t nbytes)
{
    uint64_t value;

    if (!__ABPReadWord(context, nbytes, &value)) {
        __ABPError(context, "EOF reading integer value");
        return NULL;
    }

    dprintf("int(%zu) - %llu\n", nbytes,
            (unsigned long long)value);

    return __ABPCreateInteger(context, value, nbytes);
}

static inline plist::Real *
__ABPReadReal(ABPContext *context, size_t nbytes)
{
    uint64_t value;

    if (!__ABPReadWord(context, nbytes, &value)) {
        __ABPError(context, "EOF reading real value");
        return NULL;
    }

    dprintf("real(%zu) - %g\n", nbytes,
            *(double *)&value);

    return __ABPCreateReal(context, value, nbytes);
}

static inline plist::Data *
__ABPReadData(ABPContext *context, size_t nbytes)
{
    plist::Data *data;
    off_t     offset;

    if (!__ABPReadLength(context, &nbytes)) {
        __ABPError(context, "EOF reading data length value");
        return NULL;
    }

    dprintf("data - %zu bytes\n", nbytes);

    offset = __ABPTell(context);
    data = __ABPCreateData(context, offset, nbytes);
    __ABPSeek(context, offset + nbytes, SEEK_SET);

    return data;
}

static inline plist::Data *
__ABPReadUid(ABPContext *context, size_t nbytes)
{
    uint64_t value = 0;

    if (!__ABPReadLength(context, &nbytes)) {
        __ABPError(context, "EOF reading UID length value");
        return NULL;
    }

    dprintf("uid - %zu bytes\n", nbytes);

    if (nbytes > 4)
        return NULL;

    if (nbytes > 0 && !__ABPReadWord(context, nbytes, &value))
        return NULL;

    return __ABPCreateUid(context, value);
}

static inline plist::String *
__ABPReadStringASCII(ABPContext *context, size_t nchars)
{
    off_t       offset;
    plist::String *string = NULL;

    if (!__ABPReadLength(context, &nchars)) {
        __ABPError(context, "EOF reading ASCII string length value");
        return NULL;
    }

    dprintf("ascii string - %zu(0x%zx) chars\n", nchars, nchars);

    offset = __ABPTell(context);
    string = __ABPCreateString(context, offset, nchars, false);
    __ABPSeek(context, offset + nchars, SEEK_SET);

    return string;
}

static inline plist::String *
__ABPReadStringUnicode(ABPContext *context, size_t nchars)
{
    size_t      offset;
    plist::String *string = NULL;

    if (!__ABPReadLength(context, &nchars)) {
        __ABPError(context, "EOF reading Unicode string length value");
        return NULL;
    }

    dprintf("unicode string - %zu chars\n", nchars);

    offset = __ABPTell(context);
    string = __ABPCreateString(context, offset, nchars, true);
    __ABPSeek(context, offset + (nchars * sizeof(unsigned short)), SEEK_SET);

    return string;
}

static plist::Array *
_ABPReadArray(ABPContext *context, size_t nitems)
{
    size_t     n;
    uint64_t  *objrefs;
    plist::Array *array = NULL;

    if (!__ABPReadLength(context, &nitems)) {
        __ABPError(context, "EOF reading array count value");
        return NULL;
    }

    objrefs = new uint64_t[nitems];
    dprintf("array - %zu items\n", nitems);

    for (n = 0; n < nitems; n++) {
        uint64_t objref;
        if (!__ABPReadReference(context, &objref)) {
            __ABPError(context, "corrupted array's object references table");
            goto fail;
        }

        dprintf("\titem #%zu: obj ref %llu\n", n,
                (unsigned long long)objref);
        objrefs[n] = objref;
    }

    array = __ABPCreateArray(context, objrefs, nitems);

fail:
    delete[] objrefs;
    return array;
}

static plist::Dictionary *
_ABPReadDictionary(ABPContext *context, size_t nitems)
{
    size_t          n;
    uint64_t       *kvrefs;
    plist::Dictionary *dict = NULL;

    if (!__ABPReadLength(context, &nitems)) {
        __ABPError(context, "EOF reading dictionary count value");
        return NULL;
    }

    kvrefs = new uint64_t[nitems * 2];

    dprintf("dictionary - %zu items\n", nitems);
    for (n = 0; n < nitems; n++) {
        uint64_t keyref;

        if (!__ABPReadReference(context, &keyref)) {
            __ABPError(context, "corrupted dictionary's key references table");
            goto fail;
        }

        kvrefs[n*2+0] = keyref;
    }

    for (n = 0; n < nitems; n++) {
        uint64_t objref;

        if (!__ABPReadReference(context, &objref)) {
            __ABPError(context, "corrupted dictionary's object references table");
            goto fail;
        }

        kvrefs[n*2+1] = objref;
    }

    dict = __ABPCreateDictionary(context, kvrefs, nitems);

fail:
    delete[] kvrefs;
    return dict;
}

static inline plist::Null *
__ABPReadNull(ABPContext *context)
{
    return __ABPCreateNull(context);
}

static inline plist::Boolean *
__ABPReadBool(ABPContext *context, bool value)
{
    return __ABPCreateBool(context, value);
}

static plist::Object *
_ABPReadObject(ABPContext *context)
{
    int byte;

    for (;;) {
        byte = __ABPReadByte(context);
        if (byte == EOF)
            return NULL;

        switch (__ABPByteToRecordType(byte)) {
            case kABPRecordTypeNull:
                return __ABPReadNull(context);
            case kABPRecordTypeBoolFalse:
                return __ABPReadBool(context, false);
            case kABPRecordTypeBoolTrue:
                return __ABPReadBool(context, true);
            case kABPRecordTypeFill:
                break;
            case kABPRecordTypeDate:
                return __ABPReadDate(context);
            case kABPRecordTypeInteger:
                return __ABPReadInteger(context, 1 << (byte & 0x0f));
            case kABPRecordTypeReal:
                return __ABPReadReal(context, 1 << (byte & 0x0f));
            case kABPRecordTypeData:
                return __ABPReadData(context, byte & 0x0f);
            case kABPRecordTypeStringASCII:
                return __ABPReadStringASCII(context, byte & 0x0f);
            case kABPRecordTypeStringUnicode:
                return __ABPReadStringUnicode(context, byte & 0x0f);
            case kABPRecordTypeUid:
                return __ABPReadUid(context, (byte & 0x0f) + 1);
            case kABPRecordTypeArray:
                return _ABPReadArray(context, byte & 0x0f);
            case kABPRecordTypeDictionary:
                return _ABPReadDictionary(context, byte & 0x0f);
            default:
                __ABPError(context, "unsupported type id");
                return NULL;
        }
    }
}

/*
 * Public Reader API
 */

bool
ABPReaderInit(ABPContext *context, ABPStreamCallBacks const *streamCallBacks,
        ABPCreateCallBacks const *callbacks)
{
    if (context == NULL || streamCallBacks == NULL || callbacks == NULL)
        return false;

    if (streamCallBacks->version != 0)
        return false;
    if (streamCallBacks->seek == NULL || streamCallBacks->read == NULL)
        return false;
    if (callbacks->version != 0)
        return false;
    if (callbacks->create == NULL)
        return false;

    memset(context, 0, sizeof(*context));
    context->streamCallBacks = *streamCallBacks;
    context->createCallBacks = *callbacks;
    context->flags           = kABPContextReader;

    return true;
}

bool
ABPReaderOpen(ABPContext *context)
{
    if (context == NULL)
        return false;

    if ((context->flags & (kABPContextOpened | kABPContextComplete)) != 0)
        return false;

    if (!__ABPReadHeader(context)) {
        __ABPError(context, "not a binary property list or corrupted header");
        return false;
    }

    if (!__ABPReadTrailer(context)) {
        __ABPError(context, "corrupted trailer");
        return false;
    }

    if (!_ABPReadOffsetTable(context)) {
        __ABPError(context, "corrupted offsets table");
        return false;
    }

    context->flags |= kABPContextOpened;

    return true;
}

bool
ABPReaderClose(ABPContext *context)
{
    if (context == NULL)
        return false;

    /* Fail if not reader or not opened. */
    if ((context->flags & kABPContextReader) == 0 ||
            (context->flags & kABPContextOpened) == 0)
        return false;

    _ABPContextFree(context);
    return true;
}

plist::Object *
ABPReadObject(ABPContext *context, uint64_t reference)
{
    plist::Object *object;

    if (context == NULL)
        return NULL;

    /* Fail if not a reader, complete, or not opened. */
    if ((context->flags & kABPContextComplete) != 0 ||
        (context->flags & kABPContextOpened) == 0)
        return NULL;

    if (reference >= context->trailer.objectsCount) {
        __ABPError(context, "reference out of range");
        return NULL;
    }

    object = context->objects[reference];
    if (object == NULL) {
        if (__ABPSeek(context, context->offsets[reference], SEEK_SET) < 0) {
            __ABPError(context, "object reference's offset out of range");
            return NULL;
        }

        object = _ABPReadObject(context);
        if (object == NULL)
            return NULL;

        context->objects[reference] = object;
    }

    return object;
}

plist::Object *
ABPReadTopLevelObject(ABPContext *context)
{
    return ABPReadObject(context, context->trailer.topLevelObject);
}
