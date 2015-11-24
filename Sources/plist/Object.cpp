/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Object.h>

using plist::Object;

char const *Object::
GetTypeName(enum Object::Type type)
{
    switch (type) {
        case kTypeInteger:    return "integer";
        case kTypeReal:       return "real";
        case kTypeString:     return "string";
        case kTypeBoolean:    return "boolean";
        case kTypeNull:       return "null";
        case kTypeArray:      return "array";
        case kTypeDictionary: return "dictionary";
        case kTypeData:       return "data";
        case kTypeDate:       return "date";
        default:              break;
    }
    return "unknown";
}
