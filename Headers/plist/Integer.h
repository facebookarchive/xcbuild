/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Integer_h
#define __plist_Integer_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class Integer : public Object {
private:
    int64_t _value;

public:
    Integer(int64_t value = 0) :
        _value(value)
    {
    }

public:
    inline int64_t value() const
    {
        return _value;
    }

    inline void setValue(int64_t value)
    {
        _value = value;
    }

public:
    inline static Integer *New(int64_t value = 0)
    {
        return new Integer(value);
    }

public:
    virtual enum Object::Type type() const
    {
        return Integer::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeInteger;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Integer const *objt = CastTo <Integer> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Integer const *obj) const
    {
        return (obj != nullptr && (obj == this || value() == obj->value()));
    }

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

}

#endif  // !__plist_Integer_h
