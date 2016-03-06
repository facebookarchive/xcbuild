/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_ARCHIVE_H
#define _LIBCAR_ARCHIVE_H

#include <car/AttributeList.h>
#include <car/Facet.h>
#include <bom/bom.h>
#include <ext/optional>

#include <string>
#include <vector>
#include <unordered_map>

namespace car {

class Archive {
public:
    typedef std::unique_ptr<struct bom_context, decltype(&bom_free)> unique_ptr_bom;
    typedef std::unique_ptr<struct bom_tree_context, decltype(&bom_tree_free)> unique_ptr_bom_tree;

private:
    unique_ptr_bom _bom;

private:
    Archive(unique_ptr_bom bom);

public:
    /*
     * The BOM backing this archive.
     */
    struct bom_context *bom() const
    { return _bom.get(); }

public:
    /*
     * Iterate all facets.
     */
    typedef void (*FacetIterator)(Archive const *archive, Facet const &facet, void *ctx);
    void facetIterate(FacetIterator iterator, void *ctx) const;

    /*
     * Iterate all renditions.
     */
    typedef void (*RenditionIterator)(Archive const *archive, AttributeList const &attributes, void *ctx);
    void renditionIterate(RenditionIterator iterator, void *ctx) const;

    /*
     * Iterate renditions for a facet.
     */
    typedef void (*FacetRenditionIterator)(Archive const *archive, Facet const *facet, AttributeList const &attributes, void *ctx);
    void facetRenditionIterate(Facet const &facet, FacetRenditionIterator iterator, void *ctx) const;

    /*
     * Add a new facet to the archive.
     */
    bool facetAdd(Facet const &facet);

public:
    /*
     * Load an existing archive from a BOM.
     */
    static ext::optional<Archive> Load(unique_ptr_bom bom);

    /*
     * Create a new archive inside a BOM.
     */
    static ext::optional<Archive> Create(unique_ptr_bom bom);
};

}

#endif /* _LIBCAR_ARCHIVE_H */
