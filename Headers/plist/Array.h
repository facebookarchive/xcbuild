/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Array_h
#define __plist_Array_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class Array : public Object {
public:
    typedef std::vector <Object *> Vector;

private:
    Vector _array;

public:
    Array()
    {
    }

    virtual ~Array()
    {
        clear();
    }

public:
    static std::unique_ptr<Array> New();

public:
    inline bool empty() const
    {
        return _array.empty();
    }

    inline size_t count() const
    {
        return _array.size();
    }

    inline Object const *value(size_t index) const
    {
        return _array[index];
    }

    inline Object *value(size_t index)
    {
        return _array[index];
    }

    template <typename T>
    inline T *value(size_t index)
    {
        return CastTo <T> (value(index));
    }

    template <typename T>
    inline T const *value(size_t index) const
    {
        return CastTo <T> (value(index));
    }

public:
    inline void remove(size_t index)
    {
        _array[index]->release();
        _array.erase(_array.begin() + index);
    }

public:
    inline void clear()
    {
        for (size_t n = 0; n < _array.size(); n++) {
            _array[n]->release();
        }
        _array.clear();
    }

public:
    inline void insert(size_t index, Object *obj)
    {
        _array.insert(_array.begin() + index, obj);
    }

    inline void set(size_t index, Object *obj)
    {
        _array[index]->release();
        _array[index] = obj;
    }

public:
    inline void append(Object *obj)
    {
        _array.push_back(obj);
    }

public:
    inline Vector::const_iterator begin() const
    {
        return _array.begin();
    }

    inline Vector::const_iterator end() const
    {
        return _array.end();
    }

public:
    static std::unique_ptr<Array> Coerce(Object const *obj);

public:
    virtual enum Object::Type type() const
    {
        return Array::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeArray;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Array const *objt = CastTo <Array> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Array const *obj) const
    {
        if (obj == nullptr)
            return false;

        if (count() != obj->count())
            return false;

        for (size_t n = 0; n < count(); n++) {
            if (!value(n)->equals(obj->value(n)))
                return false;
        }

        return true;
    }

public:
    void merge(Array const *array);
};

}

#endif  // !__plist_Array_h
