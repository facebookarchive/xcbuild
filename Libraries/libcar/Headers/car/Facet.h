/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_FACET_H
#define _LIBCAR_FACET_H

#include <car/AttributeList.h>
#include <ext/optional>

#include <string>

namespace car {

class Archive;

/*
 * A facet represents an entry in an archive, identifiy by name.
 * Facets can have multiple renditions, or versions of the facet.
 */
class Facet {
private:
    Archive const *_archive;
    std::string    _name;

public:
    Facet(Archive const *archive, std::string const &name);

public:
    /*
     * The archive containing the facet.
     */
    Archive const *archive() const
    { return _archive; }

    /*
     * The name of the facet.
     */
    std::string const &name() const
    { return _name; }

public:
    /*
     * The attributes associated with the facet.
     */
    ext::optional<AttributeList> attributes() const;

    /*
     * Iterate renditions for a facet.
     */
    typedef void (*RenditionIterator)(Facet const *facet, AttributeList const &attributes, void *ctx);
    void renditionIterate(RenditionIterator iterator, void *ctx) const;

public:
    /*
     * Print debugging information about the facet.
     */
    void dump() const;

public:
    /*
     * Load an existing facet with the given name.
     */
    static ext::optional<Facet> Load(Archive const *archive, std::string const &name);

    /*
     * Create a new facet with the provided name and attributes
     */
    static ext::optional<Facet> Create(Archive *archive, std::string const &name, AttributeList const &attributes);
};

}

#endif /* _LIBCAR_FACET_H */
