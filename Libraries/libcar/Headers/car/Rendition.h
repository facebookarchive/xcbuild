/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_RENDITION_H
#define _LIBCAR_RENDITION_H

#include <car/AttributeList.h>
#include <ext/optional>

namespace car {

class Archive;

/*
 * Represents a specific variant of a facet.
 */
class Rendition {
private:
    Archive const *_archive;
    AttributeList  _attributes;

private:
    Rendition(Archive const *archive, AttributeList const &attributes);

public:
    struct Properties {
        char file_name[129];
        int modification_time;

        int width;
        int height;
        float scale;
    };

public:
    /*
     * The archive this rendition is in.
     */
    Archive const *archive() const
    { return _archive; }

    /*
     * The attributes that describe the rendition.
     */
    AttributeList const &attributes() const
    { return _attributes; }

public:
    /*
     * Information about the rendition.
     */
    Properties properties() const;

    /*
     * The rendition pixel data.
     */
    void *copyData(size_t *data_len) const;

public:
    /*
     * Dump a description of the rendition. For debugging.
     */
    void dump() const;

public:
    /*
     * Load an existing rendition matching the provided attributes.
     */
    static ext::optional<Rendition> Load(car::Archive const *archive, car::AttributeList const &attributes);

    /*
     * Create a new rendition with the given properties.
     */
    static ext::optional<Rendition> Create(car::Archive *archive, car::AttributeList const &attributes, Properties const &properties, void *data, size_t data_len);
};

}

#endif /* _LIBCAR_RENDITION_H */

