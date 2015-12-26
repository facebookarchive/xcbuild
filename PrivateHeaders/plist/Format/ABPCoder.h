/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_ABPCoder_h
#define __plist_Format_ABPCoder_h

#include <plist/Object.h>
#include <plist/String.h>
#include <plist/Dictionary.h>
#include <plist/Format/abplist-format.h>

#include <unordered_map>
#include <unordered_set>

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
    kABPRecordTypeArray,
    kABPRecordTypeDictionary
} ABPRecordType;

typedef struct _ABPStreamCallBacks {
    int      version;
    void    *opaque;

    void (*close)(void *);
    off_t (*seek)(void *, off_t, int);
    ssize_t (*read)(void *, void *, size_t);
    ssize_t (*write)(void *, void const *, size_t);
} ABPStreamCallBacks;

typedef struct _ABPCreateCallBacks {
    int      version;
    void    *opaque;

    plist::Object *(*create)(void *, ABPRecordType, void *, void *, void *);
    void (*error)(void *, char const *);
} ABPCreateCallBacks;

typedef struct _ABPProcessCallBacks {
    int      version;
    void    *opaque;

    bool   (*process)(void *, plist::Object const **);
} ABPProcessCallBacks;

struct _ABPContext {
    unsigned                flags;
    abplist_header_t        header;
    abplist_trailer_t       trailer;
    uint64_t               *offsets;
    plist::Object         **objects;
    ABPStreamCallBacks      streamCallBacks;
    std::unordered_map<plist::Object const *, int>                      references;
    std::unordered_map<plist::Object const *, plist::Object const *>    mappings;
    std::unordered_set<plist::Object const *>                           written;
    std::unordered_map<plist::Dictionary const *, std::unordered_map<int, plist::String *>> keyStrings;
    union {
        ABPCreateCallBacks  createCallBacks;
        ABPProcessCallBacks processCallBacks;
    };
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

/* Writer */

bool ABPWriterInit(ABPContext *context,
        ABPStreamCallBacks const *streamCallBacks,
        ABPProcessCallBacks const *callbacks);

bool ABPWriterOpen(ABPContext *context);
bool ABPWriterFinalize(ABPContext *context);
bool ABPWriterClose(ABPContext *context);

bool ABPWriteTopLevelObject(ABPContext *context, plist::Object const *object);

#ifdef __cplusplus
}
#endif

#endif  /* !__plist_Format_ABPCoder_h */
