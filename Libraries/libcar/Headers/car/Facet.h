/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_FACET_H
#define _LIBCAR_FACET_H

#include <car/AttributeList.h>
#include <ext/optional>

#include <string>

namespace car {

class Archive;
class Rendition;

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
     * Iterate renditions for a facet.
     */
    void renditionIterate(Archive const *archive, std::function<void(Rendition const &)> const &iterator) const;

public:
    /*
     * Print debugging information about the facet.
     */
    void dump() const;

public:
    /*
     * Create a facet with the provided name and attributes
     */
    static Facet Create(std::string const &name, AttributeList const &attributes);
};

}

#endif /* _LIBCAR_FACET_H */
