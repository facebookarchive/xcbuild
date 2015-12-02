
/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Keys_Unpack_h
#define __plist_Keys_Unpack_h

#include <plist/Object.h>
#include <plist/Dictionary.h>

namespace plist {
namespace Keys {

class Unpack {
private:
    std::string                      _name;
    Dictionary const                *_dict;
    std::unordered_set<std::string> *_seen;
    std::vector<std::string>         _errors;

public:
    Unpack(std::string const &name, Dictionary const *dict, std::unordered_set<std::string> *seen);

public:
    Object const *value(std::string const &key);

    template<typename T>
    T const *cast(std::string const &key)
    {
        Object const *object = value(key);
        if (object == nullptr) {
            return nullptr;
        }

        if (T const *value = CastTo<T>(object)) {
            return value;
        }

        _errors.push_back(_name + " key " + key + " was type " + object->typeName() + "; expected " + Object::GetTypeName(T::Type()));
        return nullptr;
    }

    template<typename T>
    std::unique_ptr<T> coerce(std::string const &key)
    {
        Object const *object = value(key);
        if (object == nullptr) {
            return nullptr;
        }

        if (std::unique_ptr<T> value = T::Coerce(object)) {
            return value;
        }

        _errors.push_back(_name + " key " + key + " was type " + object->typeName() + "; could not coerce to " + Object::GetTypeName(T::Type()));
        return nullptr;
    }

public:
    bool complete(bool check);
    std::string errors() const;
};


}
}

#endif  // !__plist_Keys_Unpack_h
