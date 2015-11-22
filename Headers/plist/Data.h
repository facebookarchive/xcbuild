/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Data_h
#define __plist_Data_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

class Data : public Object {
private:
    std::vector <uint8_t> _value;

public:
    Data(std::vector <uint8_t> const &value = std::vector <uint8_t> ()) :
        _value(value)
    {
    }

    Data(std::vector <uint8_t> &&value) :
        _value(value)
    {
    }

    Data(std::string const &value)
    {
        libutil::Base64::Decode(value, _value);
    }

    Data(void const *bytes, size_t length)
    {
        setValue(bytes, length);
    }

public:
    inline std::vector <uint8_t> const &value() const
    {
        return _value;
    }

    inline void setValue(std::vector <uint8_t> const &value)
    {
        _value = value;
    }

    inline void setValue(std::vector <uint8_t> &&value)
    {
        _value = value;
    }

public:
    inline void setValue(void const *bytes, size_t length)
    {
        _value.resize(length);
        std::memcpy(&_value[0], bytes, length);
    }


public:
    inline void setBase64Value(std::string const &value)
    {
        libutil::Base64::Decode(value, _value);
    }

    inline std::string base64Value() const
    {
        return libutil::Base64::Encode(_value);
    }

public:
    inline static Data *New(std::vector <uint8_t> const &value = std::vector <uint8_t> ())
    {
        return new Data(value);
    }

    inline static Data *New(std::vector <uint8_t> &&value)
    {
        return new Data(value);
    }

    inline static Data *New(std::string const &value)
    {
        return new Data(value);
    }

    inline static Data *New(void const *bytes, size_t length)
    {
        return new Data(bytes, length);
    }

public:
    virtual enum Object::Type type() const
    {
        return Data::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeData;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Data const *objt = CastTo <Data> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Data const *obj) const
    {
        return (obj != nullptr && (obj == this || value() == obj->value()));
    }
};

}

#endif  // !__plist_Data_h
