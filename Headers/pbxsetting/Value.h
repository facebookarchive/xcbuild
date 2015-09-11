// Copyright 2013-present Facebook. All Rights Reserved.

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
