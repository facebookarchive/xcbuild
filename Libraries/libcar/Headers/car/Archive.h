/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef _LIBCAR_ARCHIVE_H
#define _LIBCAR_ARCHIVE_H

#include <car/AttributeList.h>
#include <bom/bom.h>
#include <ext/optional>

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace car {

class Rendition;

/*
 * An archive within a BOM file holding facets and their renditions.
 */
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
    typedef void (*FacetIterator)(Archive const *archive, std::string const &name, void *ctx);
    void facetIterate(FacetIterator iterator, void *ctx) const;

    /*
     * Iterate all renditions.
     */
    void renditionIterate(std::function<void(Rendition const &)> const &iterator) const;

public:
    /*
     * Print debug information about the archive.
     */
    void dump() const;

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
