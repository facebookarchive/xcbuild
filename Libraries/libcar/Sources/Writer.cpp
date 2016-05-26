/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <car/Writer.h>
#include <car/Facet.h>
#include <car/Rendition.h>
#include <car/car_format.h>

#include <string.h>

#include <random>

using car::Writer;

Writer::
Writer(unique_ptr_bom bom) :
    _bom(std::move(bom))
{
}

ext::optional<Writer> Writer::
Create(unique_ptr_bom bom)
{
    return Writer(std::move(bom));
}

void Writer::
addFacet(Facet const &facet)
{
    _facetValues.insert({facet.name(), facet});
}

void Writer::
addRendition(Rendition const &rendition)
{
    auto identifier = rendition.attributes().get(car_attribute_identifier_identifier);
    if (identifier != ext::nullopt) {
        _renditionValues.insert({*identifier, rendition});
    }
}

void Writer::
Write()
{
    // Write CARHEADER
    struct car_header *header = (struct car_header *)malloc(sizeof(struct car_header));
    if (header == NULL) {
        return;
    }

    strncpy(header->magic, "RATC", 4);
    header->ui_version = 0x131; // todo
    header->storage_version = 0xC; // todo
    header->storage_timestamp = time(NULL); // todo
    header->rendition_count = 0;
    strncpy(header->file_creator, "asset catalog compiler\n", sizeof(header->file_creator));
    strncpy(header->other_creator, "version 1.0", sizeof(header->other_creator));

    std::random_device device;
    std::uniform_int_distribution<int> distribution = std::uniform_int_distribution<int>(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
    for (size_t i = 0; i < sizeof(header->uuid); i++) {
        header->uuid[i] = distribution(device);
    }

    header->associated_checksum = 0; // todo
    header->schema_version = 4; // todo
    header->color_space_id = 1; // todo
    header->key_semantics = 1; // todo

    int header_index = bom_index_add(_bom.get(), header, sizeof(struct car_header));
    bom_variable_add(_bom.get(), car_header_variable, header_index);
    free(header);

    // Write KEYFORMAT
    struct car_key_format *keyfmt = *_keyfmt;
    size_t keyfmt_size = sizeof(struct car_key_format) + (keyfmt->num_identifiers * sizeof(keyfmt->identifier_list[0]));
    int key_format_index = bom_index_add(_bom.get(), keyfmt, keyfmt_size);
    bom_variable_add(_bom.get(), car_key_format_variable, key_format_index);

    // Write FACETKEYS
    struct bom_tree_context *facets_tree_context = bom_tree_alloc_empty(_bom.get(), car_facet_keys_variable);
    if (facets_tree_context != NULL) {
        for (auto const &item : _facetValues ) {
            auto facet_value = item.second.Write();
            bom_tree_add(facets_tree_context,
                reinterpret_cast<const void *>(item.first.c_str()),
                item.first.size(),
                reinterpret_cast<const void *>(facet_value.data()),
                facet_value.size());
        }
        bom_tree_free(facets_tree_context);
    }

    // Write RENDITIONS
    struct bom_tree_context *renditions_tree_context = bom_tree_alloc_empty(_bom.get(), car_renditions_variable);
    if (renditions_tree_context != NULL) {
        for (auto const &item : _renditionValues ) {
            auto attributes_value = item.second.attributes().Write(keyfmt->num_identifiers, keyfmt->identifier_list);
            auto rendition_value = item.second.Write();
            bom_tree_add(renditions_tree_context,
                reinterpret_cast<const void *>(attributes_value.data()),
                attributes_value.size(),
                reinterpret_cast<const void *>(rendition_value.data()),
                rendition_value.size());
        }
        bom_tree_free(renditions_tree_context);
    }
}

