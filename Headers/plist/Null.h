/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Null_h
#define __plist_Null_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class Null : public Object {
private:
    Null()
    {
    }

public:
    static void *operator new(size_t) throw()
    {
        return nullptr;
    }

    static void operator delete(void *)
    {
    }

public:
    virtual void release() const
    {
    }

private:
    static Null const kNull;

public:
    inline static Null *New()
    {
        return const_cast <Null *> (&kNull);
    }

public:
    static std::unique_ptr<Null> Coerce(Object const *obj);

public:
    virtual enum Object::Type type() const
    {
        return Null::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeNull;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Null const *objt = CastTo <Null> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Null const *obj) const
    {
        return (obj != nullptr && obj == this);
    }
};

}

#endif  // !__plist_Null_h
