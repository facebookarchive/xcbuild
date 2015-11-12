/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Real_h
#define __plist_Real_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class Real : public Object {
private:
    double _value;

public:
    Real(double value = 0.0) :
        _value(value)
    {
    }

public:
    inline double value() const
    {
        return _value;
    }

    inline void setValue(double value)
    {
        _value = value;
    }

public:
    inline static Real *New(double value = 0.0)
    {
        return new Real(value);
    }

public:
    virtual enum Object::Type type() const
    {
        return Real::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeReal;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Real const *objt = CastTo <Real> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Real const *obj) const
    {
        return (obj != nullptr && (obj == this || value() == obj->value()));
    }

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

}

#endif  // !__plist_Real_h
