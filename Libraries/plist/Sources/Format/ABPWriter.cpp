/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/ABPWriterPrivate.h>
#include <plist/Format/Encoding.h>
#include <plist/Objects.h>

using plist::Format::Encoding;
using plist::Format::Encodings;
using plist::Object;
using plist::Boolean;
using plist::Real;
using plist::Integer;
using plist::String;
using plist::Data;
using plist::Date;
using plist::Null;
using plist::UID;
using plist::Array;
using plist::Dictionary;

enum {
    kABPWriteObjectTopLevel  = (1 << 0), /* Object is top level. */
    kABPWriteObjectReference = (1 << 1), /* Write only the reference, even if not written yet. */
    kABPWriteObjectValue     = (1 << 2)  /* Write only the value if not yet written. */
};

static bool
_ABPWritePreflightObject(ABPContext *context, Object const *object,
        uint32_t flags);

static bool
_ABPWriteObject(ABPContext *context, Object const *object, uint32_t flags);

static bool
__ABPWriteOffsetTable(ABPContext *context)
{
    uint64_t n;
    uint64_t highestOffset;

    /* Update offset in trailer. */
    context->trailer.offsetTableEndOffset = __ABPTell(context);

    /* Highest offset is offset table offset */
    highestOffset = context->trailer.offsetTableEndOffset;

    /* Estimate offset integer size. */
    if (highestOffset > UINT32_MAX) {
        context->trailer.offsetIntByteSize = sizeof(uint64_t);
    } else if (highestOffset > UINT16_MAX) {
        context->trailer.offsetIntByteSize = sizeof(uint32_t);
    } else if (highestOffset > UINT8_MAX) {
        context->trailer.offsetIntByteSize = sizeof(uint16_t);
    } else {
        context->trailer.offsetIntByteSize = sizeof(uint8_t);
    }

    /* Write out the offsets. */
    for (n = 0; n < context->trailer.objectsCount; n++) {
        if (!__ABPWriteOffset(context, context->offsets[n]))
            return false;
    }

    return true;
}

static bool
__ABPWriteNullWithArg(ABPContext *context, void *ignored)
{
    return __ABPWriteByte(context, __ABPRecordTypeToByte(kABPRecordTypeNull, 0));
}

static bool
__ABPWriteBool(ABPContext *context, Boolean const *value)
{
    return __ABPWriteByte(context, __ABPRecordTypeToByte(
                value->value() ? kABPRecordTypeBoolTrue :
                kABPRecordTypeBoolFalse, 0));
}

static bool
__ABPWriteDate(ABPContext *context, Date const *date)
{
    /* Reference time is 2001/1/1 */
    static uint64_t const ReferenceTimestamp = 978307200;
    double at;
    uint64_t value;

    /* Write object type. */
    if (!__ABPWriteByte(context, __ABPRecordTypeToByte(kABPRecordTypeDate, 0)))
        return false;

    at = date->unixTimeValue() - ReferenceTimestamp;
    /* HACK(strager): We should not rely on C's representation of double. */
    memcpy(&value, &at, 8);
    return __ABPWriteWord(context, 8, value);
}

static bool
__ABPWriteInteger(ABPContext *context, Integer const *integer)
{
    int     nbits = 3;
    int64_t value = integer->value();

    if ((value & 0xFF) == value) {
        nbits = 0;
    } else if ((value & 0xFFFF) == value) {
        nbits = 1;
    } else if ((value & 0xFFFFFFFF) == value) {
        nbits = 2;
    }

    /* Write object type. */
    if (!__ABPWriteByte(context, __ABPRecordTypeToByte(kABPRecordTypeInteger,
                    nbits)))
        return false;

    /* Write word. */
    return __ABPWriteWord(context, 1 << nbits, value);
}

static bool
__ABPWriteReal(ABPContext *context, Real *real)
{
    int         nbits;
    uint64_t    uvalue;
    double      value = real->value();
    float       value32 = value;

    if ((double)value32 == value) {
        /* HACK(strager): We should not rely on C's representation of float. */
        memcpy(&uvalue, &value32, 4);
        nbits = 2;
    } else {
        /* HACK(strager): We should not rely on C's representation of double. */
        memcpy(&uvalue, &value, 8);
        nbits = 3;
    }

    /* Write object type. */
    if (!__ABPWriteByte(context, __ABPRecordTypeToByte(kABPRecordTypeReal,
                    nbits)))
        return false;

    return __ABPWriteWord(context, 1 << nbits, uvalue);
}

static bool
__ABPWriteData(ABPContext *context, Data *data)
{
    size_t length;

    /* Write the object type and the length. */
    length = data->value().size();
    if (!__ABPWriteTypeAndLength(context, kABPRecordTypeData, length))
        return false;

    /* Write contents. */
    return (__ABPWriteBytes(context, data->value().data(), length) == length);
}

static bool
__ABPWriteUID(ABPContext *context, plist::UID *uid)
{
    uint32_t value;
    size_t   nbytes = sizeof(uint32_t);

    value = uid->value();
    if ((value & 0xFF) == value) {
        nbytes = 1;
    } else if ((value & 0xFFFF) == value) {
        nbytes = 2;
    }

    /* Write the object type and the length. */
    if (!__ABPWriteTypeAndLength(context, kABPRecordTypeUid, nbytes))
        return false;

    /* Write word. */
    return __ABPWriteWord(context, nbytes, value);
}

static bool
__ABPWriteStringASCII(ABPContext *context, char const *chars, size_t nchars)
{
    /* Write the object type and the length. */
    if (!__ABPWriteTypeAndLength(context, kABPRecordTypeStringASCII, nchars))
        return false;

    return (nchars == 0 || __ABPWriteBytes(context, chars, nchars) == nchars);
}

static bool
__ABPWriteStringUnicode(ABPContext *context, uint16_t const *chars, size_t nchars)
{
    /* Write the object type and the length. */
    if (!__ABPWriteTypeAndLength(context, kABPRecordTypeStringUnicode, nchars))
        return false;

    if (nchars == 0)
        return true;

    return (__ABPWriteBytes(context, chars, nchars * sizeof(uint16_t)) == (size_t)(nchars * sizeof(uint16_t)));
}

static bool
__ABPWriteString(ABPContext *context, String *string)
{
    bool success;

    bool ascii = true;
    for (uint8_t c : string->value()) {
        if (c > 0x80) {
            ascii = false;
            break;
        }
    }

    if (ascii) {
        success = __ABPWriteStringASCII(context, string->value().c_str(), string->value().size());
    } else {
        std::vector<uint8_t> buffer = std::vector<uint8_t>(string->value().begin(), string->value().end());
        buffer = Encodings::Convert(buffer, Encoding::UTF8, Encoding::UTF16BE);
        success = __ABPWriteStringUnicode(context, reinterpret_cast<uint16_t *>(buffer.data()), buffer.size() / sizeof(uint16_t));
    }

    return success;
}

/*
 * Workaround for Dictionaries not having keys with identity. Map from integer key
 * index to key value to use as the identity when writing the key object out.
 */
static String *
__ABPDictionaryKeyString(ABPContext *context, Dictionary const *dict, int key)
{
    std::unordered_map<int, String *> *map = &context->keyStrings[dict];

    auto it = map->find(key);
    if (it != map->end()) {
        return it->second;
    } else {
        auto string = String::New(dict->key(key));
        map->insert({ key, string.get() });
        return string.release();
    }
}

static inline void
__ABPWriteArrayReferences(ABPContext *context, Array const *array)
{
    for (int i = 0; i < array->count(); ++i) {
        _ABPWriteObject(context, array->value(i), kABPWriteObjectReference);
    }
}

static inline void
__ABPWriteArrayValues(ABPContext *context, Array const *array)
{
    for (int i = 0; i < array->count(); ++i) {
        _ABPWriteObject(context, array->value(i), kABPWriteObjectValue);
    }
}

static bool
__ABPWriteArray(ABPContext *context, Array const *array)
{
    int length = array->count();

    /* Write the object type and the length. */
    if (!__ABPWriteTypeAndLength(context, kABPRecordTypeArray, length))
        return false;

    if (length > 0) {
        /* Write all the references and all the values. */
        __ABPWriteArrayReferences(context, array);
        __ABPWriteArrayValues(context, array);
    }

    return true;
}

static void
__ABPWriteDictionaryReferences(ABPContext *context, Dictionary const *dict)
{
    for (int i = 0; i < dict->count(); ++i) {
        String *key = __ABPDictionaryKeyString(context, dict, i);
        _ABPWriteObject(context, key, kABPWriteObjectReference);
    }

    for (int i = 0; i < dict->count(); ++i) {
        _ABPWriteObject(context, dict->value(i), kABPWriteObjectReference);
    }
}

static void
__ABPWriteDictionaryValues(ABPContext *context, Dictionary const *dict)
{
    for (int i = 0; i < dict->count(); ++i) {
        String *key = __ABPDictionaryKeyString(context, dict, i);
        _ABPWriteObject(context, key, kABPWriteObjectValue);
    }

    for (int i = 0; i < dict->count(); ++i) {
        _ABPWriteObject(context, dict->value(i), kABPWriteObjectValue);
    }
}

static bool
__ABPWriteDictionary(ABPContext *context, Dictionary const *dict)
{
    int length = dict->count();

    /* Write the object type and the length. */
    if (!__ABPWriteTypeAndLength(context, kABPRecordTypeDictionary, length))
        return false;

    if (length > 0) {
        /* Write all the reference and value pairs. */
        __ABPWriteDictionaryReferences(context, dict);
        __ABPWriteDictionaryValues(context, dict);
    }

    return true;
}

/* Preflighting */

static inline void
__ABPWritePreflightArrayReferences(ABPContext *context, Array const *array)
{
    for (int i = 0; i < array->count(); ++i) {
        _ABPWritePreflightObject(context, array->value(i), 0);
    }
}

static bool
__ABPWritePreflightArray(ABPContext *context, Array const *array)
{
    /* Preflight all the values. */
    __ABPWritePreflightArrayReferences(context, array);
    return true;
}

static void
__ABPWritePreflightDictionaryReferences(ABPContext *context,
        Dictionary const *dict)
{
    for (int i = 0; i < dict->count(); ++i) {
        String *key = __ABPDictionaryKeyString(context, dict, i);
        _ABPWritePreflightObject(context, key, 0);
    }

    for (int i = 0; i < dict->count(); ++i) {
        _ABPWritePreflightObject(context, dict->value(i), 0);
    }
}

static bool
__ABPWritePreflightDictionary(ABPContext *context, Dictionary const *dict)
{
    /* Preflight all the keys and values. */
    __ABPWritePreflightDictionaryReferences(context, dict);
    return true;
}

/*
 * Process an object, calls the user callback in order to
 * return a suitable object for the encoding; the object
 * is added to the mapping table if different from the original
 * and a reference is associated with the object; valid references
 * are always non-zero.
 * If 'userProcess' is set, when an object is cached, the object
 * field is set to null and only the reference number is returned.
 */
static bool
_ABPWriterProcessObject(ABPContext *context, Object const **object, uint32_t *refno,
        bool userProcess)
{
    Object const *newObject;
    Object const *origObject = *object;

    /* Is this object already mapped? */
    auto mit = context->mappings.find(origObject);
    if (mit != context->mappings.end()) {
        newObject = mit->second;
    } else {
        newObject = origObject;

        /* Process the object for mapping. */
        if (userProcess && (*(context->processCallBacks.process))(
                    context->processCallBacks.opaque, &newObject)) {
            enum Object::Type type = newObject->type();
            /* Cache mapping, but do so only if newObject is different
             * than the origObject or origObject is not a container.
             */
            if (newObject != origObject || (type != Array::Type() &&
                                            type != Dictionary::Type() &&
                                            type != Null::Type() &&
                                            type != Boolean::Type())) {
                context->mappings.insert({ origObject, newObject });
            }
        } else {
            /*
             * The user callback can't handle this object, supposedly because
             * we can.
             */
        }
    }

    assert(newObject != NULL);

    /* Is this new object already written? */
    auto it = context->references.find(newObject);
    if (it != context->references.end()) {
        *refno = it->second;

        /*
         * Yes, invalidate newObject so the caller knows that this is
         * a reference.
         */
        if (userProcess) {
            newObject = NULL;
        }
    } else {
        /*
         * No, add a new reference for this newObject.
         */
        *refno = context->references.size();
        context->references.insert({ newObject, *refno });
    }

    /* Return the new object to the user. */
    *object = newObject;

    return true;
}

/*
 * Preflight objects so that we emit them ordered; this is needed in
 * order to create binary property lists compatible with standard format;
 * failing to do so will result in an a property list that cannot be
 * opened by any tool (although you can simply use our own which are
 * more than happy to process a "broken" plist).
 */
static bool
_ABPWritePreflightObject(ABPContext *context, Object const *object,
        uint32_t flags)
{
    enum Object::Type type;
    bool  success = true;
    uint32_t   refno   = 0;

    /*
     * Process the object; if returned object is non-null, it
     * is the first time the object has been seen, otherwise
     * only refno is valid.
     */
    if (!_ABPWriterProcessObject(context, &object, &refno, true))
        return false;

    if (object == NULL)
        return true;

    /* If this is the top level object, store reference in the header. */
    if (flags & kABPWriteObjectTopLevel) {
        context->trailer.topLevelObject = refno;
    }

    /* Add this object to the offsets table. */
    if (refno >= context->trailer.objectsCount) {
        context->trailer.objectsCount = refno + 1;
    }

    /* Preflight dictionaries and arrays. */
    type = object->type();
    if (type == Array::Type()) {
        success = __ABPWritePreflightArray(context, (Array const *)object);
    } else if (type == Dictionary::Type()) {
        success = __ABPWritePreflightDictionary(context,
                (Dictionary const *)object);
    }

    if (!success) abort();
    return success;
}

static bool
_ABPWriteObject(ABPContext *context, Object const *object, uint32_t flags)
{
    enum Object::Type type;
    off_t        offset;
    bool         success;
    uint32_t     refno = 0;

    /*
     * Process the object; if returned object is non-null, it
     * is the first time the object has been seen, otherwise
     * only refno is valid.
     */
    if (!_ABPWriterProcessObject(context, &object, &refno, false))
        return false;

    if (context->written.find(object) != context->written.end()) {
        if (flags & kABPWriteObjectValue)
            return true;

        flags |= kABPWriteObjectReference;
    }

    if (flags & kABPWriteObjectReference) {
        return __ABPWriteReference(context, refno);
    }

    /* Cache written object. */
    context->written.insert(object);

    type = object->type();
    offset = __ABPTell(context);

    static const struct { enum Object::Type type; void *cb; } typeToCB[] = {
        { Array::Type(), (void *)__ABPWriteArray },
        { Dictionary::Type(), (void *)__ABPWriteDictionary },
        { Integer::Type(), (void *)__ABPWriteInteger },
        { Real::Type(), (void *)__ABPWriteReal },
        { String::Type(), (void *)__ABPWriteString },
        { Boolean::Type(), (void *)__ABPWriteBool },
        { Null::Type(), (void *)__ABPWriteNullWithArg },
        { Data::Type(), (void *)__ABPWriteData },
        { Date::Type(), (void *)__ABPWriteDate },
        { UID::Type(), (void *)__ABPWriteUID },
    };

    bool (*writer)(ABPContext *, Object const *) = nullptr;
    for (auto const &it : typeToCB) {
        if (it.type == type) {
            writer = (bool(*)(ABPContext *, Object const *))it.cb;
            break;
        }
    }

    if (writer) {
        success = writer(context, object);
    } else {
        success = false;
    }

    if (success) {
        /* Update offsets table. */
        context->offsets[refno] = offset;
    }

    return success;
}

/*
 * Public Writer API
 */

bool
ABPWriterInit(ABPContext *context, ABPStreamCallBacks const *streamCallBacks,
        ABPProcessCallBacks const *callbacks)
{
    if (context == NULL || streamCallBacks == NULL || callbacks == NULL)
        return false;

    if (streamCallBacks->version != 0)
        return false;
    if (streamCallBacks->seek == NULL || streamCallBacks->write == NULL)
        return false;
    if (callbacks->version != 0)
        return false;
    if (callbacks->process == NULL)
        return false;

    memset(context, 0, sizeof(*context));
    context->streamCallBacks  = *streamCallBacks;
    context->processCallBacks = *callbacks;
    context->flags            = 0;
    context->references       = std::unordered_map<plist::Object const *, int>();
    context->mappings         = std::unordered_map<plist::Object const *, plist::Object const *>();
    context->written          = std::unordered_set<plist::Object const *>();
    context->keyStrings       = std::unordered_map<plist::Dictionary const *, std::unordered_map<int, plist::String *>>();

    return true;
}

bool
ABPWriterOpen(ABPContext *context)
{
    if (context == NULL)
        return false;

    if (context->flags & (kABPContextReader | kABPContextOpened |
                kABPContextComplete))
        return false;

    /* Initialize the header struct and write it. */
    memset(&context->header, 0, sizeof(context->header));
    memcpy(context->header.magic, ABPLIST_MAGIC,
           sizeof(context->header.magic));
    memcpy(context->header.version, ABPLIST_VERSION,
           sizeof(context->header.version));

    if (!__ABPWriteHeader(context)) {
        return false;
    }

    /* Initialize the trailer struct. */
    memset(&context->trailer, 0, sizeof(context->trailer));

    context->flags |= kABPContextOpened;

    return true;
}

bool
ABPWriterFinalize(ABPContext *context)
{
    if (context == NULL)
        return false;

    /* Fail if a reader, not opened or not complete. */
    if ((context->flags & kABPContextReader) != 0 ||
            (context->flags & (kABPContextOpened | kABPContextComplete)) !=
             (kABPContextOpened | kABPContextComplete))
        return false;

    /* If we finalized already, do nothing. */
    if (context->flags & kABPContextFlushed)
        return true;

    /* Write offset table and trailer. */
    if (!__ABPWriteOffsetTable(context))
        return false;

    if (!__ABPWriteTrailer(context, false))
        return false;

    context->flags |= kABPContextFlushed;
    return true;
}

bool
ABPWriterClose(ABPContext *context)
{
    if (context == NULL)
        return false;

    /* Fail if a reader or not opened. */
    if ((context->flags & kABPContextReader) != 0 ||
            (context->flags & kABPContextOpened) == 0)
        return false;

    if ((context->flags & kABPContextFlushed) == 0) {
        if (!ABPWriterFinalize(context))
            return false;
    }

    for (auto const &map : context->keyStrings) {
        for (auto const &item : map.second) {
            item.second->release();
        }
    }

    _ABPContextFree(context);
    return true;
}

bool
ABPWriteTopLevelObject(ABPContext *context, Object const *object)
{
    bool success;

    if (context == NULL || object == NULL)
        return false;

    /* Fail if a reader, complete, or not opened. */
    if ((context->flags & (kABPContextReader | kABPContextComplete)) != 0 ||
            (context->flags & kABPContextOpened) == 0)
        return false;

    /*
     * Preflight objects... the standard seems to support only binary plists
     * whose offsets are increasing *sigh*
     */
    success = _ABPWritePreflightObject(context, object,
            kABPWriteObjectTopLevel);
    if (success) {
        /* Estimate the object reference size. */
        uint64_t nrefs = context->references.size();
        if (nrefs > UINT32_MAX) {
            context->trailer.objectRefByteSize = sizeof(uint64_t);
        } else if (nrefs > UINT16_MAX) {
            context->trailer.objectRefByteSize = sizeof(uint32_t);
        } else if (nrefs > UINT8_MAX) {
            context->trailer.objectRefByteSize = sizeof(uint16_t);
        } else {
            context->trailer.objectRefByteSize = sizeof(uint8_t);
        }

        /* Allocate enough space for the offsets table. */
        context->offsets = (uint64_t *)calloc(
                context->trailer.objectsCount, sizeof(uint64_t));
        if (context->offsets == NULL)
            return false;

        /* Write all the objects. */
        success = _ABPWriteObject(context, object, kABPWriteObjectTopLevel);
        if (success) {
            context->flags |= kABPContextComplete;
        }
    }

    return success;
}
