/* Copyright 2013-present Facebook. All Rights Reserved. */

/* Unfortunate, but needed for car_attribute_identifier. */
#include <car/car_format.h>
#include <ext/optional>

#include <vector>
#include <unordered_map>

namespace std {
template<> struct hash<enum car_attribute_identifier> {
    size_t operator()(enum car_attribute_identifier identifier) const
    {
        return (size_t)identifier;
    }
};
}

namespace car {

/*
 * Stores a list of attribute identifiers and values. Used as a key for
 * renditions, which are uniquely identified by their attribute list.
 */
class AttributeList {
private:
    std::unordered_map<enum car_attribute_identifier, uint16_t> _values;

public:
    AttributeList(std::unordered_map<enum car_attribute_identifier, uint16_t> const &values);

public:
    bool operator==(AttributeList const &rhs) const;
    bool operator!=(AttributeList const &rhs) const;

public:
    /*
     * Get the value of an attribute.
     */
    ext::optional<uint16_t> get(enum car_attribute_identifier identifier) const;

    /*
     * Set the value of an attribute. Appends the attribute if not found.
     */
    void set(enum car_attribute_identifier identifier, uint16_t value);

    /*
     * Iterate over the contents of the attribute list. Unordered.
     */
    template<typename T>
    void iterate(T iterator) const
    {
        for (auto const &entry : _values) {
            iterator(entry.first, entry.second);
        }
    }

    /*
     * The number of attributes in the list.
     */
    size_t count() const;

public:
    /*
     * Print debugging information about the list.
     */
    void dump() const;

public:
    /*
     * Load an attribute list from buffers of identifiers and values.
     */
    static AttributeList Load(
        size_t count,
        enum car_attribute_identifier *identifiers,
        uint16_t *values);
};

}

namespace std {
template<> struct hash<car::AttributeList>
{
    /*
     * Suport using an AttributeList as a key.
     */
    size_t operator()(car::AttributeList const &attributes) const;
};
}

