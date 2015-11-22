/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Date_h
#define __plist_Date_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class Date : public Object {
private:
    struct tm _value;

public:
    Date(struct tm const &tm) :
        _value(tm)
    {
    }

    Date(std::string const &value)
    {
        libutil::ISODate::Decode(value, _value);
    }

    Date(uint64_t value = 0)
    {
        libutil::UnixTime::Decode(value, _value);
    }

public:
    inline struct tm const &value() const
    {
        return _value;
    }

    inline void setValue(struct tm const &value)
    {
        _value = value;
    }

public:
    inline void setStringValue(std::string const &value)
    {
        libutil::ISODate::Decode(value, _value);
    }

    inline std::string stringValue() const
    {
        return libutil::ISODate::Encode(_value);
    }

public:
    inline void setUnixTimeValue(uint64_t value)
    {
        libutil::UnixTime::Decode(value, _value);
    }

    inline uint64_t unixTimeValue() const
    {
        return libutil::UnixTime::Encode(_value);
    }

public:
    inline static Date *New(struct tm const &value = tm ())
    {
        return new Date(value);
    }

    inline static Date *New(std::string const &value)
    {
        return new Date(value);
    }

    inline static Date *New(uint64_t value)
    {
        return new Date(value);
    }

public:
    virtual enum Object::Type type() const
    {
        return Date::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeDate;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Date const *objt = CastTo <Date> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Date const *obj) const
    {
        return (obj != nullptr && (obj == this || unixTimeValue() == obj->unixTimeValue()));
    }
};

}

#endif  // !__plist_Date_h
