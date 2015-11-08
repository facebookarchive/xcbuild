/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Objects_h
#define __plist_Objects_h

#include <plist/Base.h>

#include <cstring>
#include <ctime>

namespace plist {

class Object {
public:
    typedef std::unique_ptr <Object> UniquePtr;

public:
    enum Type {
        kTypeNone,
        kTypeInteger,
        kTypeReal,
        kTypeString,
        kTypeBoolean,
        kTypeNull,
        kTypeArray,
        kTypeDictionary,
        kTypeData,
        kTypeDate
    };

protected:
    Object()
    {
    }

    virtual ~Object()
    {
    }

public:
    virtual Type type() const = 0;

public:
    virtual void release()
    {
        delete this;
    }

public:
    virtual Object *copy() const = 0;
    virtual bool equals(Object const *obj) const
    {
        return (obj == this);
    }

public:
    static Object *Parse(std::string const &path);
    static Object *Parse(std::string const &path,
            error_function const &error);

    static Object *Parse(std::FILE *fp);
    static Object *Parse(std::FILE *fp,
            error_function const &error);

public:
    void dump(FILE *fp = stderr, size_t indent = 0) const;

protected:
    static inline void Indent(FILE *fp, size_t n);
    static inline std::string QuoteString(std::string const &s);

protected:
    friend class Array;
    friend class Dictionary;
    virtual void dump1(FILE *fp, size_t indent, size_t cindent) const = 0;

public:
    static inline enum Object::Type Type()
    {
        return Object::kTypeNone;
    }

public:
    inline char const *typeName() const
    {
        return GetTypeName(type());
    }

public:
    static char const *GetTypeName(enum Object::Type type);
};

template <typename T>
static inline T *CastTo(Object *obj)
{
    return (obj != nullptr && obj->type() == T::Type()) ? static_cast <T *> (obj) : nullptr;
}


template <typename T>
static inline T const *CastTo(Object const *obj)
{
    return (obj != nullptr && obj->type() == T::Type()) ? static_cast <T const *> (obj) : nullptr;
}

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

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

class Boolean : public Object {
private:
    bool _value;

private:
    Boolean(bool value) :
        _value(value)
    {
    }

protected:
    static void *operator new(size_t) throw()
    {
        return nullptr;
    }

    static void operator delete(void *)
    {
    }

public:
    virtual void release()
    {
    }

private:
    static Boolean const kTrue;
    static Boolean const kFalse;

protected:
    friend Boolean *CastTo <> (Object *);
    friend Boolean const *CastTo <> (Object const *);
    static Boolean *New(Object const *object);

public:
    inline static Boolean *New(bool value)
    {
        return const_cast <Boolean *> (value ? &kTrue : &kFalse);
    }

public:
    inline bool value() const
    {
        return _value;
    }

public:
    virtual enum Object::Type type() const
    {
        return Boolean::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeBoolean;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const;
    virtual bool equals(Boolean const *obj) const
    {
        return (obj != nullptr && (obj == this || value() == obj->value()));
    }

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

template <>
inline Boolean *CastTo(Object *obj)
{
    if (obj == nullptr)
        return nullptr;
    else if (obj->type() == Boolean::Type())
        return static_cast <Boolean *> (obj);
    else
        return Boolean::New(obj);
}

template <>
inline Boolean const *CastTo(Object const *obj)
{
    if (obj == nullptr)
        return nullptr;
    else if (obj->type() == Boolean::Type())
        return static_cast <Boolean const *> (obj);
    else
        return Boolean::New(obj);
}

class Null : public Object {
private:
    Null()
    {
    }

private:
    static void *operator new(size_t) throw()
    {
        return nullptr;
    }

    static void operator delete(void *)
    {
    }

public:
    virtual void release()
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

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

class Array : public Object {
public:
    typedef std::vector <Object *> Vector;

private:
    Vector _array;

public:
    Array()
    {
    }

protected:
    virtual ~Array()
    {
        clear();
    }

public:
    inline static Array *New()
    {
        return new Array;
    }

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

    template <typename T>
    inline T const *value(size_t index) const
    {
        return CastTo <T> (value(index));
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

public:
    static Array *Parse(std::string const &path);
    static Array *Parse(std::string const &path,
            error_function const &error);

    static Array *Parse(std::FILE *fp);
    static Array *Parse(std::FILE *fp,
            error_function const &error);

public:
    virtual void dump1(FILE *fp, size_t, size_t cindent) const;
};

class Dictionary : public Object {
private:
    typedef std::vector <std::string> KeyVector;
    typedef std::map <std::string, Object *> Map;

private:
    KeyVector _keys;
    Map       _map;

public:
    Dictionary()
    {
    }

protected:
    virtual ~Dictionary()
    {
        clear();
    }

public:
    inline static Dictionary *New()
    {
        return new Dictionary;
    }

public:
    inline bool empty() const
    {
        return _map.empty();
    }

    inline size_t count() const
    {
        return _map.size();
    }

    inline std::string const &key(size_t index) const
    {
        return _keys[index];
    }

    inline Object const *value(size_t index) const
    {
        return (index < _keys.size()) ? value(_keys[index]) : nullptr;
    }

    template <typename T>
    inline T const *value(size_t index) const
    {
        return CastTo <T> (value(index));
    }

    inline Object const *value(std::string const &key) const
    {
        Map::const_iterator i = _map.find(key);
        return (i != _map.end()) ? i->second : nullptr;
    }

    template <typename T>
    inline T const *value(std::string const &key) const
    {
        return CastTo <T> (value(key));
    }

public:
    inline void clear()
    {
        for (auto I : _map) {
            I.second->release();
        }
        _keys.clear();
        _map.clear();
    }

public:
    inline void set(std::string const &key, Object *obj)
    {
        remove(key);
        _keys.push_back(key);
        _map.insert(std::make_pair(key, obj));
    }

    inline void remove(std::string const &key, bool release = true)
    {
        Map::iterator I;

        if ((I = _map.find(key)) != _map.end()) {
            if (release) {
                I->second->release();
            }
            _map.erase(I);
            _keys.erase(std::find(_keys.begin(), _keys.end(), key));
        }
    }

public:
    inline KeyVector::const_iterator begin() const
    {
        return _keys.begin();
    }

    inline KeyVector::const_iterator end() const
    {
        return _keys.end();
    }

public:
    virtual enum Object::Type type() const
    {
        return Dictionary::Type();
    }

    static inline enum Object::Type Type()
    {
        return Object::kTypeDictionary;
    }

public:
    virtual Object *copy() const;
    virtual bool equals(Object const *obj) const
    {
        if (Object::equals(obj))
            return true;

        Dictionary const *objt = CastTo <Dictionary> (obj);
        return (objt != nullptr && equals(objt));
    }

    virtual bool equals(Dictionary const *obj) const
    {
        if (obj == nullptr)
            return false;

        if (count() != obj->count())
            return false;

        for (Map::const_iterator i = _map.begin(); i != _map.end(); ++i) {
            if (!value(i->first)->equals(obj->value(i->first)))
                return false;
        }

        return true;
    }

public:
    void merge(Dictionary const *dict, bool replace = true);

protected:
    virtual void dump1(FILE *fp, size_t, size_t cindent) const;

public:
    static Dictionary *Parse(std::string const &path);
    static Dictionary *Parse(std::string const &path,
            error_function const &error);

    static Dictionary *Parse(std::FILE *fp);
    static Dictionary *Parse(std::FILE *fp,
            error_function const &error);

public:
    static Dictionary *ParseSimpleXML(std::string const &path);
    static Dictionary *ParseSimpleXML(std::string const &path,
            error_function const &error);

    static Dictionary *ParseSimpleXML(std::FILE *fp);
    static Dictionary *ParseSimpleXML(std::FILE *fp,
            error_function const &error);
};

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

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

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

protected:
    virtual void dump1(FILE *fp, size_t indent, size_t) const;
};

template <typename T>
T *Copy(T const *object)
{
    if (object == nullptr)
        return nullptr;
    
    if (auto o = CastTo <T> (object))
        return CastTo <T> (o->copy());

    return nullptr;
}

static inline char const *
GetObjectTypeName(int type)
{
    if (type == plist::String::Type())
        return "string";
    if (type == plist::Boolean::Type())
        return "boolean";
    if (type == plist::Array::Type())
        return "array";
    if (type == plist::Dictionary::Type())
        return "dictionary";
    if (type == plist::Integer::Type())
        return "integer";
    if (type == plist::Real::Type())
        return "real";
    if (type == plist::Data::Type())
        return "data";
    if (type == plist::Date::Type())
        return "date";
    if (type == plist::Null::Type())
        return "null";
    return "unknown";
}

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

#endif  // !__plist_Objects_h
