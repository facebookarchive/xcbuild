/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/Facet.h>

using car::Facet;

Facet::
Facet(std::string const &name, AttributeList const &attributes) :
    _name      (name),
    _attributes(attributes)
{
}

void Facet::
dump() const
{
    fprintf(stderr, "Facet: %s\n", _name.c_str());
    _attributes.dump();
}

