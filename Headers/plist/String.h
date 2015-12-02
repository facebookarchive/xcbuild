/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_String_h
#define __plist_String_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class String : public Object {
private:
    std::string _value;

public:
    String(std::string const &value = std::string()) :
        _value(value)
    {
    }

    String(std::string &&value) :
        _value(value)
    {
    }

public:
    inline std::string const &value() const
    {
        return _value;
    }

    inline void setValue(std::string const &value)
    {
        _value = value;
    }

    inline void setValue(std::string &&value)
    {
        _value = value;
    }

public:
    inline static String *New(std::string const &value = std::string())
    {
        return new String(value);
    }

    inline static String *New(std::string &&value)
    {
        return new String(value);
    }

public:
    static std::unique_ptr<String> Coerce(Object const *obj);

public:
    virtual enum Object::Type type() const
    {
        return String::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeString;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        String const *objt = CastTo <String> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(String const *obj) const
    {
        return (obj != nullptr && (obj == this || value() == obj->value()));
    }
};

}

#endif  // !__plist_String_h
