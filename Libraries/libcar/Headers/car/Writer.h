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
#include <car/Facet.h>
#include <car/Rendition.h>
#include <bom/bom.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <ext/optional>

namespace car {

class Facet;
class Rendition;

/*
 * An archive within a BOM file holding facets and their renditions.
 */
class Writer {
public:
    typedef std::unique_ptr<struct bom_context, decltype(&bom_free)> unique_ptr_bom;

private:
    unique_ptr_bom _bom;
    std::unordered_map<std::string, Facet> _facets;
    std::unordered_multimap<uint16_t, Rendition> _renditions;

private:
    Writer(unique_ptr_bom bom);

public:
    /*
     * The BOM backing this archive.
     */
    struct bom_context *bom() const
    { return _bom.get(); }

public:
    /*
     * Add a facet to the archive.
     */
    void addFacet(Facet const &facet);

    /*
     * Add a rendition for a facet, allow lazy loading of data.
     */
    void addRendition(Rendition const &rendition);

public:
    /*
     * Create a new archive inside a BOM.
     */
    static ext::optional<Writer> Create(Writer::unique_ptr_bom bom);

public:
    /*
     * Serialize and write to BOM.
     */
     void write() const;
};

}

#endif /* _LIBCAR_WRITER_H */
