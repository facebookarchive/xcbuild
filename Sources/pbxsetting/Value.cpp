// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxsetting/Value.h>

using pbxsetting::Value;

bool Value::Entry::
operator==(Entry const &rhs) const
{
    if (type != rhs.type) {
        return false;
    } else if (type == String) {
        return string == rhs.string;
    } else if (type == Value::Entry::Value) {
        return *value == *rhs.value;
    } else {
        assert(false);
    }
}

Value::
Value(std::vector<Entry> const &entries) :
    _entries(entries)
{
}

Value::
~Value()
{
}

bool Value::
operator==(Value const &rhs) const
{
    return _entries == rhs._entries;
}

struct ParseResult {
    bool found;
    size_t end;
    Value value;
};

static ParseResult
ParseValue(std::string const &value, size_t from, char end = '\0')
{
    bool found = true;
    std::vector<Value::Entry> entries;

    size_t length;
    size_t search_offset = from;
    size_t append_offset = from;

    do {
        size_t to = end != '\0' ? value.find(end, search_offset) : value.size();
        if (to == std::string::npos) {
            return { .found = false, .end = from, .value = Value(entries) };
        }

        size_t pno  = value.find("$(", search_offset);
        size_t cbo  = value.find("${", search_offset);
        size_t open = (pno < cbo ? pno : cbo);
        if (open == std::string::npos || open >= to) {
            length = to - append_offset;
            if (length > 0) {
                entries.push_back({ .type = Value::Entry::String, .string = value.substr(append_offset, length) });
            }

            return { .found = true, .end = to, .value = Value(entries) };
        }

        ParseResult result = ParseValue(value, open + 2, (open == pno ? ')' : '}'));
        if (result.found) {
            length = open - append_offset;
            if (length > 0) {
                entries.push_back({ .type = Value::Entry::String, .string = value.substr(append_offset, length) });
            }

            entries.push_back({ .type = Value::Entry::Value, .value = std::make_shared<Value>(result.value) });

            append_offset = result.end + 1;
            search_offset = result.end + 1;
        } else {
            search_offset += 2;
        }
    } while (true);
}

Value Value::
Parse(std::string const &value)
{
    return ParseValue(value, 0).value;
}

Value const &Value::
Empty(void)
{
    static Value *value = new Value({ });
    return *value;
}

