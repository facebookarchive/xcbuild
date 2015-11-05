/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxsetting_Value_h
#define __pbxsetting_Value_h

#include <pbxsetting/Base.h>
#include <plist/plist.h>

namespace pbxsetting {

class Value {
public:
    struct Entry {
        enum Type {
            String,
            Value,
        };

        bool operator==(Entry const &rhs) const;

        Type type;
        std::string string;
        std::shared_ptr<class Value> value;
    };

    std::vector<Entry> _entries;

public:
    Value(std::vector<Entry> const &entries);
    ~Value();

public:
    bool operator==(Value const &rhs) const;
    Value operator+(Value const &rhs) const;

public:
    std::vector<Entry> const &entries() const
    { return _entries; }

public:
    std::string
    raw() const;

public:
    static Value
    Parse(std::string const &value);
    static Value
    String(std::string const &value);
    static Value
    Variable(std::string const &value);
    static Value
    FromObject(plist::Object const *object);
    static Value const &
    Empty(void);
};

}

#endif  // !__pbxsetting_Value_h
