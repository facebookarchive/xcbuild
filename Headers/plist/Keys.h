/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Keys_h
#define __plist_Keys_h

#include <plist/Object.h>
#include <plist/Dictionary.h>

namespace plist {

typedef std::map <std::string, enum Object::Type> KeyTypeMap;
typedef KeyTypeMap::value_type KeyType;

template <typename T>
static inline KeyType MakeKey(char const *name)
{ return std::make_pair(name, T::Type()); }

void
WarnUnhandledKeyMap(plist::Dictionary const *dict, char const *name,
        KeyTypeMap const &keys);

inline void
WarnUnhandledKeyMap(plist::Dictionary const *dict,
        KeyTypeMap const &keys)
{
    WarnUnhandledKeyMap(dict, nullptr, keys);
}

template <typename... TypedKeys>
static void
WarnUnhandledKeys(plist::Dictionary const *dict, char const *name,
        TypedKeys const &... typedKeys)
{
    KeyType const tk[] = { typedKeys... };

    KeyTypeMap keys;
    for (size_t n = 0; n < sizeof...(typedKeys); n++) {
        keys.insert(tk[n]);
    }

    WarnUnhandledKeyMap(dict, name, keys);
}

template <typename... TypedKeys>
static void
WarnUnhandledKeys(plist::Dictionary const *dict,
        TypedKeys const &... typedKeys)
{
    WarnUnhandledKeys(dict, nullptr, typedKeys...);
}

}

#endif  // !__plist_Keys_h
