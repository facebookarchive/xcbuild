/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Format_Info_h
#define __plist_Format_Info_h

#include <plist/Format/Type.h>
#include <plist/Format/Binary.h>
#include <plist/Format/XML.h>
#include <plist/Format/ASCII.h>
#include <plist/Format/Strings.h>
#include <plist/Format/SimpleXML.h>

namespace plist {
namespace Format {

class Info {
private:
    union Contents {
        Contents() { };

        Binary    binary;
        XML       xml;
        ASCII     ascii;
        Strings   strings;
        SimpleXML simpleXML;
    };

private:
    Type     _type;
    Contents _contents;

private:
    Info(Type type, Contents const &contents);

public:
    inline Type type() const
    { return _type; }

public:
    template<typename T>
    inline T const *format() const
    {
        if (_type == T::Type()) {
            return reinterpret_cast<T const *>(&_contents);
        } else {
            return nullptr;
        }
    }

public:
    template<typename T>
    static Info Create(T const &format)
    {
        Contents contents;
        *reinterpret_cast<T *>(&contents) = format;
        return Info(T::Type(), contents);
    }
};

}
}

#endif  // !__plist_Format_Info_h
