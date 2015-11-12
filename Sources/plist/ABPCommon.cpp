/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
