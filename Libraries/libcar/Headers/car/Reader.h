/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef _LIBCAR_READER_H
#define _LIBCAR_READER_H

#include <car/AttributeList.h>
#include <bom/bom.h>
#include <ext/optional>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace car {

class Facet;
class Rendition;

/*
 * An archive within a BOM file holding facets and their renditions.
 */
class Reader {
public:
    typedef std::unique_ptr<struct bom_context, decltype(&bom_free)> unique_ptr_bom;
    typedef std::unique_ptr<struct bom_tree_context, decltype(&bom_tree_free)> unique_ptr_bom_tree;

private:
    unique_ptr_bom _bom;

private:
    Reader(unique_ptr_bom bom);

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
    void facetIterate(std::function<void(Facet const &)> const &facet) const;

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
    static ext::optional<Reader> Load(unique_ptr_bom bom);

    /*
     * Create a new archive inside a BOM.
     */
    static ext::optional<Reader> Create(unique_ptr_bom bom);
};

}

#endif /* _LIBCAR_READER_H */
