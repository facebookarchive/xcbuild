/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <car/Facet.h>
#include <car/Rendition.h>
#include <car/Reader.h>

#include <cstring>
#include <cstdlib>

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

    ext::optional<car::AttributeList> attributes = this->attributes();
    if (attributes) {
        attributes->dump();
    }
}

void Facet::
renditionIterate(Reader const *archive, std::function<void(Rendition const &)> const &iterator) const
{
    ext::optional<car::AttributeList> attributes = this->attributes();
    if (!attributes) {
        return;
    }

    archive->renditionIterate([&](Rendition const &rendition) {
        ext::optional<uint16_t> facet_identifier = attributes->get(car_attribute_identifier_identifier);
        ext::optional<uint16_t> rendition_identifier = rendition.attributes().get(car_attribute_identifier_identifier);
        if (rendition_identifier && facet_identifier && *rendition_identifier == *facet_identifier) {
            iterator(rendition);
        }
    });
}

Facet Facet::
Create(std::string const &name, AttributeList const &attributes)
{
    return Facet(name, attributes);
}

