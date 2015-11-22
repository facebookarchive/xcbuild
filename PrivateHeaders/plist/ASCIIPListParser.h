/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_ASCIIPListParser_h
#define __plist_ASCIIPListParser_h

#include <plist/Object.h>
#include <plist/String.h>

#include <stack>
#include <string>

#if __cplusplus
extern "C" {
#endif

/** Type */

typedef enum _ASCIIContextState {
    kASCIIContextStateParsing = 0,
    kASCIIContextStateDone,
    kASCIIContextStateAborted,
} ASCIIContextState;

typedef enum _ASCIIValueState {
    kASCIIValueStateInit = 0,
    kASCIIValueStateDictionary,
    kASCIIValueStateDictionaryValue,
    kASCIIValueStateArray,
} ASCIIValueState;

typedef struct _ASCIIPListParserContext {
    plist::Object              *root;
    int                         level;

    ASCIIValueState             state;
    std::stack<ASCIIValueState> stateStack;

    plist::Object              *container;
    std::stack<plist::Object *> containerStack;

    plist::String              *key;
    std::stack<plist::String *> keyStack;

    ASCIIContextState           contextState;
    std::string                 error;
} ASCIIPListParserContext;


/** Lifecycle */

void
ASCIIPListParserContextInit(ASCIIPListParserContext *context);

void
ASCIIPListParserContextFree(ASCIIPListParserContext *context);


/** Contents */

plist::Object *
ASCIIPListParserCopyRoot(ASCIIPListParserContext const *context);


/** State */

bool
ASCIIPListParserIsAborted(ASCIIPListParserContext *context);

void
ASCIIPListParserAbort(ASCIIPListParserContext *context, std::string const &error);

bool
ASCIIPListParserIsDone(ASCIIPListParserContext *context);

bool
ASCIIPListParserFinish(ASCIIPListParserContext *context);


/** Level */

int
ASCIIPListParserGetLevel(ASCIIPListParserContext const *context);

void
ASCIIPListParserIncrementLevel(ASCIIPListParserContext *context);

void
ASCIIPListParserDecrementLevel(ASCIIPListParserContext *context);


/** Parsing */

bool
ASCIIPListParserIsArray(ASCIIPListParserContext const *context);

bool
ASCIIPListParserArrayBegin(ASCIIPListParserContext *context);

bool
ASCIIPListParserArrayEnd(ASCIIPListParserContext *context);

bool
ASCIIPListParserIsDictionary(ASCIIPListParserContext const *context);

bool
ASCIIPListParserDictionaryBegin(ASCIIPListParserContext *context);

bool
ASCIIPListParserDictionaryEnd(ASCIIPListParserContext *context);

bool
ASCIIPListParserStoreKey(ASCIIPListParserContext *context, plist::String *key);

bool
ASCIIPListParserStoreValue(ASCIIPListParserContext *context, plist::Object *value);

#ifdef __cplusplus
}
#endif

#endif  // !__plist_ASCIIPListParser_h
