/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef _LIBCAR_WRITER_H
#define _LIBCAR_WRITER_H

#include <car/AttributeList.h>
#include <bom/bom.h>
#include <ext/optional>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#import <experimental/string_view>

namespace car {

class Facet;
class Rendition;

/*
 * An archive within a BOM file holding facets and their renditions.
 */
class Writer {
public:
    typedef std::unique_ptr<struct bom_context, decltype(&bom_free)> unique_ptr_bom;
    typedef std::unique_ptr<struct bom_tree_context, decltype(&bom_tree_free)> unique_ptr_bom_tree;

private:
    unique_ptr_bom _bom;
    ext::optional<struct car_key_format*> _keyfmt;
    std::unordered_map<std::string, Facet const &> _facetValues;
    std::unordered_multimap<uint16_t, Rendition &> _renditionValues;

private:
    Writer(unique_ptr_bom bom);

public:
    /*
     * The BOM backing this archive.
     */
    struct bom_context *bom() const
    { return _bom.get(); }

    /*
     * The key format
     */
    struct car_key_format * keyfmt() const
    { return *_keyfmt; }
    struct car_key_format * &keyfmt()
    { return *_keyfmt; }

public:
    /*
     * Add a Facet
     */
    void addFacet(Facet const &);

    /*
     * Add a Rendition for a Facet, allow lazy loading of data
     */
    void addRendition(Rendition &);

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
     * Lookup a Facet by name
     */
    ext::optional<car::Facet> lookupFacet(std::string name) const;

    /*
     * Lookup Rendition list for a Facet
     */
    std::vector<car::Rendition> lookupRenditions(Facet const &) const;

public:
    /*
     * Print debug information about the archive.
     */
    void dump() const;

public:
    /*
     * Create a new archive inside a BOM.
     */
    static ext::optional<Writer> Create(Writer::unique_ptr_bom bom);

public:
    /*
     * Serialize and write to BOM
     */
     void Write();
};

}

#endif /* _LIBCAR_WRITER_H */
