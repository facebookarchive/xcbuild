/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Operations_Adjustment_h
#define __plist_Operations_Adjustment_h

#include <plist/Base.h>
#include <plist/Object.h>
#include <iostream>

namespace plist {

class Adjustment {
public:
    enum class Type {
        Extract,
        Replace,
        Insert,
        Remove,
    };

private:
    Type                                 _type;
    std::string                          _path;
    std::unique_ptr<plist::Object const> _value;

public:
    Adjustment(Type type, std::string const &path, std::unique_ptr<plist::Object const> value);

public:
    Type type() const
    { return _type; }
    std::string const &path() const
    { return _path; }
    plist::Object const *value() const
    { return _value.get(); }
};

}

#endif  // !__plist_Operations_Adjustment_h
