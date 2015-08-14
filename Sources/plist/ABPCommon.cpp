/* Copyright (c) 2013-present Facebook. All rights reserved. */

#include <plist/ABPCoderPrivate.h>

#include <cstring>

size_t
ABPGetObjectsCount(ABPContext *context)
{
    return (context != NULL ? context->trailer.objectsCount : 0);
}

void
_ABPContextFree(ABPContext *context)
{
    if (context->objects != NULL) {
        for (size_t n = 0; n < context->trailer.objectsCount; n++) {
            if (context->objects[n] != NULL) {
                context->objects[n]->release();
            }
        }
        delete[] context->objects;
    }

    if (context->offsets != NULL) {
        delete[] context->offsets;
    }

    memset(context, 0, sizeof(*context));
}
