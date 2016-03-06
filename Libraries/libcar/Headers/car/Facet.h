/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_FACET_H
#define _LIBCAR_FACET_H

#include <car/AttributeList.h>

#include <string>

namespace car {

/*
 * A facet represents a logical entry.
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
    std::string &name()
    { return _name; }
    std::string const &name() const
    { return _name; }

    /*
     * The attributes describing the facet.
     */
    AttributeList &attributes()
    { return _attributes; }
    AttributeList const &attributes() const
    { return _attributes; }

public:
    /*
     * Print debugging information about the facet.
     */
    void dump() const;
};

}

#endif /* _LIBCAR_FACET_H */
