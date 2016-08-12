/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef _LIBCAR_FACET_H
#define _LIBCAR_FACET_H

#include <car/AttributeList.h>
#include <ext/optional>

#include <string>

namespace car {

class Reader;
class FacetReference;

/*
 * A facet represents an entry in an archive, identifiy by name.
 * Facets can have multiple renditions, or versions of the facet.
 */
class Facet {
private:
    std::string   _name;
    AttributeList _attributes;

public:
    Facet(std::string const &name, AttributeList const &attributes);

public:
    /*
     * The name of the facet.
     */
    std::string const &name() const
    { return _name; }

    /*
     * The attributes associated with the facet.
     */
    AttributeList attributes() const
    { return _attributes; }

public:
    /*
     * Serialize the rendition for writing to a file.
     */
    std::vector<uint8_t> write() const;

public:
    /*
     * Print debugging information about the facet.
     */
    void dump() const;

public:
    /*
     * Create a facet with the provided name and attributes
     */
    static Facet Create(
        std::string const &name,
        AttributeList const &attributes);

    /*
     * Load an existing facet.
     */
    static Facet Load(FacetReference const &reference);
};

/*
 * A facet reference points to a facet within an archive.
 */
class FacetReference {
private:
    char const *_name;
    size_t      _nameSize;

public:
    void const *_value;
    size_t      _valueSize;

public:
    FacetReference(char const *name, size_t nameSize, void const *value, size_t valueSize);

public:
    char const *name() const
    { return _name; }
    size_t nameSize() const
    { return _nameSize; }

public:
    void const *value() const
    { return _value; }
    size_t valueSize() const
    { return _valueSize; }

public:
    AttributeList::Identifier identifier() const;
};

}

#endif /* _LIBCAR_FACET_H */
