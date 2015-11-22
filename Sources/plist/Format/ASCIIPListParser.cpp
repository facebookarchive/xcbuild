/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Format/ASCIIPListParser.h>
#include <plist/Objects.h>

using plist::Object;
using plist::String;
using plist::Data;
using plist::Array;
using plist::Dictionary;

void
ASCIIPListParserContextInit(ASCIIPListParserContext *context)
{
    context->root = NULL;
    context->level = 0;

    context->state = kASCIIValueStateInit;
    context->container = NULL;
    context->key = NULL;

    context->contextState = kASCIIContextStateParsing;
}

void
ASCIIPListParserContextFree(ASCIIPListParserContext *context)
{
    while (!context->containerStack.empty()) {
        Object *container = context->containerStack.top();
        container->release();
        context->containerStack.pop();
    }

    if (context->container != NULL) {
        context->container->release();
        context->container = NULL;
    }

    while (!context->keyStack.empty()) {
        String *key = context->keyStack.top();
        key->release();
        context->keyStack.pop();
    }

    if (context->key != NULL) {
        context->key->release();
        context->key = NULL;
    }

    if (context->root != NULL) {
        context->root->release();
        context->root = NULL;
    }
}

bool
ASCIIPListParserIsAborted(ASCIIPListParserContext *context)
{
    return context->contextState == kASCIIContextStateAborted;
}

bool
ASCIIPListParserIsDone(ASCIIPListParserContext *context)
{
    return context->contextState == kASCIIContextStateDone;
}

int
ASCIIPListParserGetLevel(ASCIIPListParserContext const *context)
{
    return context->level;
}

void
ASCIIPListParserIncrementLevel(ASCIIPListParserContext *context)
{
    context->level++;
}

void
ASCIIPListParserDecrementLevel(ASCIIPListParserContext *context)
{
    if (context->level != 0) {
        context->level--;
    }
}

void
ASCIIPListParserAbort(ASCIIPListParserContext *context, std::string const &error)
{
    context->error = error;
    context->contextState = kASCIIContextStateAborted;
}

static bool
ASCIIPListParserPush(ASCIIPListParserContext *context, ASCIIValueState state, Object *container, String *key)
{
    if (ASCIIPListParserIsAborted(context))
        return false;

    /* If valid state, push, otherwise just set the new state. */
    if (context->state != kASCIIValueStateInit) {
        /* Push the old state */
        context->containerStack.push(context->container);
        context->keyStack.push(context->key);
        context->stateStack.push(context->state);
    }

    context->state = state;

    if (container != NULL) {
        context->container = container->copy();
    } else {
        context->container = NULL;
    }

    if (key != NULL) {
        context->key = (String *)key->copy();
    } else {
        context->key = NULL;
    }

    return true;
}

static bool
ASCIIPListParserPop(ASCIIPListParserContext *context)
{
    Object *container;
    String *key;
    int     count = context->stateStack.size();

    if (count == 0) {
        if (context->state == kASCIIValueStateInit)
            return false; /* Underflow! */

        /* Reset current state. */
        if (context->container != NULL) {
            context->container->release();
            context->container = NULL;
        }

        if (context->key != NULL) {
            context->key->release();
            context->key = NULL;
        }

        context->state = kASCIIValueStateInit;
        return true;
    }

    /* Pop state */
    context->state = context->stateStack.top();
    context->stateStack.pop();

    if (context->container != NULL) {
        context->container->release();
    }
    context->container = context->containerStack.top();
    context->containerStack.pop();

    if (context->key != NULL) {
        context->key->release();
    }
    context->key = context->keyStack.top();
    context->keyStack.pop();

    return true;
}

/*
 * Generic container handling.
 */
static bool
__ASCIIPListParserContainerBegin(ASCIIPListParserContext *context, Object *object)
{
    ASCIIValueState state;

    if (object->type() == Array::Type()) {
        state = kASCIIValueStateArray;
    } else {
        state = kASCIIValueStateDictionary;
    }

    if (!ASCIIPListParserPush(context, state, object, NULL)) {
        ASCIIPListParserAbort(context, "Cannot push the current state.");
        return false;
    }

    return true;
}

static bool
__ASCIIPListParserStoreValue(ASCIIPListParserContext *context, String *key, Object *value)
{
    if (context->container == NULL) {
        if (context->key != NULL) {
            ASCIIPListParserAbort(context, "Storing key/value pair with no container.");
            return false;
        }

        /* Make current value the root. */
        if (context->root != NULL) {
            ASCIIPListParserAbort(context, "Double root.");
            return false;
        }

        context->root = value->copy();
        return true;
    }

    if (key != NULL) {
        if (context->container->type() != Dictionary::Type()) {
            ASCIIPListParserAbort(context, "Storing key/value with no dictionary container.");
            return false;
        }

        ((Dictionary *)context->container)->set(key->value(), value->copy());

        context->state = kASCIIValueStateDictionary;
    } else {
        if (context->container->type() != Array::Type()) {
            ASCIIPListParserAbort(context, "Storing value with no array container.");
            return false;
        }

        ((Array *)context->container)->append(value->copy());

        context->state = kASCIIValueStateArray;
    }

    return true;
}

static bool
__ASCIIPListParserContainerEnd(ASCIIPListParserContext *context, bool isArray)
{
    String        *key;
    Object        *value;
    bool           success;

    /* Check state is consistant. */
    if (context->state != (isArray ? kASCIIValueStateArray : kASCIIValueStateDictionary)) {
        ASCIIPListParserAbort(context, "Closing array/dictionary in wrong state.");
        return false;
    }

    if (context->container == NULL) {
        ASCIIPListParserAbort(context, "Closing non-opened container.");
        return false;
    }

    value = context->container->copy();
    if (value == NULL) {
        ASCIIPListParserAbort(context, "Couldn't copy container.");
        return false;
    }

    /* Is this the right type of container? */
    if (value->type() != (isArray ? Array::Type() : Dictionary::Type())) {
        ASCIIPListParserAbort(context, "Closing wrong kind of container.");
        return false;
    }

    if (!ASCIIPListParserPop(context)) {
        ASCIIPListParserAbort(context, "Parser stack underflow.");
        return false;
    }

    /* Take ownership of the saved key. */
    key = context->key;
    context->key = NULL;

    success = __ASCIIPListParserStoreValue(context, key, value);

    if (key != NULL) {
        key->release();
    }
    value->release();

    return success;
}

bool
ASCIIPListParserArrayBegin(ASCIIPListParserContext *context)
{
    Array   *array;

    array = Array::New();
    if (array == NULL) {
        ASCIIPListParserAbort(context, "Cannot create an array.");
        return false;
    }

    bool success = __ASCIIPListParserContainerBegin(context, array);
    return success;
}

bool
ASCIIPListParserArrayEnd(ASCIIPListParserContext *context)
{
    return __ASCIIPListParserContainerEnd(context, true);
}

bool
ASCIIPListParserDictionaryBegin(ASCIIPListParserContext *context)
{
    Dictionary  *dict;

    dict = Dictionary::New();
    if (dict == NULL) {
        ASCIIPListParserAbort(context, "Cannot create a dictionary.");
        return false;
    }

    bool success = __ASCIIPListParserContainerBegin(context, dict);
    return success;
}

bool
ASCIIPListParserDictionaryEnd(ASCIIPListParserContext *context)
{
    return __ASCIIPListParserContainerEnd(context, false);
}

/*
 * Store the key of the current dictionary.
 */
bool
ASCIIPListParserStoreKey(ASCIIPListParserContext *context, String *key)
{
    String *copy;

    if (key == NULL) {
        copy = String::New();
    } else {
        copy = (String *)key->copy();
    }

    if (copy == NULL) {
        ASCIIPListParserAbort(context, "Cannot create copy of the key.");
        return false;
    }

    if (context->state != kASCIIValueStateDictionary) {
        ASCIIPListParserAbort(context, "Storing key in wrong state.");
        return false;
    }

    if (context->key != NULL) {
        context->key->release();
    }
    context->key = copy;

    context->state = kASCIIValueStateDictionaryValue;
    return true;
}

bool
ASCIIPListParserStoreValue(ASCIIPListParserContext *context, Object *value)
{
    bool success;

    success = __ASCIIPListParserStoreValue(context, context->key, value);
    if (context->key != NULL) {
        context->key->release();
        context->key = NULL;
    }

    if (context->state == kASCIIValueStateDictionaryValue) {
        context->state = kASCIIValueStateDictionary;
    }

    return success;
}

bool
ASCIIPListParserIsDictionary(ASCIIPListParserContext const *context)
{
    return context->state == kASCIIValueStateDictionary;
}

bool
ASCIIPListParserIsArray(ASCIIPListParserContext const *context)
{
    return context->state == kASCIIValueStateArray;
}

bool
ASCIIPListParserFinish(ASCIIPListParserContext *context)
{
    if (ASCIIPListParserIsDone(context))
        return true;

    context->contextState = kASCIIContextStateDone;
    return true;
}

Object *
ASCIIPListParserCopyRoot(ASCIIPListParserContext const *context)
{
    if (context->root == NULL)
        return NULL;

    return context->root->copy();
}

