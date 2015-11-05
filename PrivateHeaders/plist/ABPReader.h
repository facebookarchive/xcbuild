/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_ABPReader_h
#define __plist_ABPReader_h

#include <plist/Objects.h>
#include <plist/abplist-format.h>

typedef struct _ABPContext ABPContext;

typedef enum _ABPRecordType {
    kABPRecordTypeInvalid = -1,
    kABPRecordTypeNull = 0,
    kABPRecordTypeBoolTrue,
    kABPRecordTypeBoolFalse,
    kABPRecordTypeFill,
    kABPRecordTypeDate,
    kABPRecordTypeInteger,
    kABPRecordTypeReal,
    kABPRecordTypeData,
    kABPRecordTypeStringASCII,
    kABPRecordTypeStringUnicode,
    kABPRecordTypeUid,
    kABPRecordTypeArray,
    kABPRecordTypeDictionary
} ABPRecordType;

typedef struct _ABPStreamCallBacks {
    int      version;
    void    *opaque;

    void (*close)(void *);
    off_t (*seek)(void *, off_t, int);
    ssize_t (*read)(void *, void *, size_t);
} ABPStreamCallBacks;

typedef struct _ABPCreateCallBacks {
    int   version;
    void *opaque;

    plist::Object * (*create)(void *, ABPRecordType, void *, void *, void *);
    void (*error)(void *, char const *);
} ABPCreateCallBacks;

struct _ABPContext {
    unsigned             flags;
    abplist_header_t     header;
    abplist_trailer_t    trailer;
    uint64_t            *offsets;
    plist::Object      **objects;
    ABPStreamCallBacks   streamCallBacks;
    ABPCreateCallBacks   createCallBacks;
};

/* Private Coder Flags */
enum {
    kABPContextReader   = (1 << 0),
    kABPContextOpened   = (1 << 1),
    kABPContextComplete = (1 << 2),
    kABPContextFlushed  = (1 << 3)
};

#ifdef __cplusplus
extern "C" {
#endif

/* Reader */

bool ABPReaderInit(ABPContext *context,
        ABPStreamCallBacks const *streamCallBacks,
        ABPCreateCallBacks const *callbacks);

bool ABPReaderOpen(ABPContext *context);
bool ABPReaderClose(ABPContext *context);

size_t ABPReaderGetObjectsCount(ABPContext *context);

plist::Object *ABPReadTopLevelObject(ABPContext *context);
plist::Object *ABPReadObject(ABPContext *context, uint64_t reference);

#ifdef __cplusplus
}
#endif

#endif  /* !__plist_ABPReader_h */
