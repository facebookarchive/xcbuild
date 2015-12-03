/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Dictionary_h
#define __plist_Dictionary_h

#include <plist/Base.h>
#include <plist/Object.h>

namespace plist {

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

    virtual ~Dictionary()
    {
        clear();
    }

public:
    static std::unique_ptr<Dictionary> New();

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

    inline Object *value(size_t index)
    {
        return (index < _keys.size()) ? value(_keys[index]) : nullptr;
    }

    template <typename T>
    inline T const *value(size_t index) const
    {
        return CastTo <T> (value(index));
    }

    template <typename T>
    inline T *value(size_t index)
    {
        return CastTo <T> (value(index));
    }

    inline Object const *value(std::string const &key) const
    {
        Map::const_iterator i = _map.find(key);
        return (i != _map.end()) ? i->second : nullptr;
    }

    inline Object *value(std::string const &key)
    {
        Map::const_iterator i = _map.find(key);
        return (i != _map.end()) ? i->second : nullptr;
    }

    template <typename T>
    inline T const *value(std::string const &key) const
    {
        return CastTo <T> (value(key));
    }

    template <typename T>
    inline T *value(std::string const &key)
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
    static std::unique_ptr<Dictionary> Coerce(Object const *obj);

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
};

}

#endif  // !__plist_Dictionary_h
