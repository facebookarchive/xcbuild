/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/DerivedDataHash.h>

#include <libutil/FSUtil.h>
#include <libutil/md5.h>

using pbxbuild::DerivedDataHash;
using libutil::FSUtil;

DerivedDataHash::
DerivedDataHash(std::string const &name, std::string const &hash) :
    _name(name),
    _hash(hash)
{
}

std::string DerivedDataHash::
derivedDataHash() const
{
    return _name + "-" + _hash;
}

std::vector<pbxsetting::Setting> DerivedDataHash::
overrideSettings() const
{
    std::string hash = derivedDataHash();

    return {
        pbxsetting::Setting::Parse("SYMROOT", "$(DERIVED_DATA_DIR)/" + hash + "/Build/Products"),
        pbxsetting::Setting::Parse("OBJROOT", "$(DERIVED_DATA_DIR)/" + hash + "/Build/Intermediates"),
    };
}

static uint64_t
hton64(uint64_t v)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    v = ((v & 0x00000000000000FFULL) << 56) |
        ((v & 0x000000000000FF00ULL) << 40) |
        ((v & 0x0000000000FF0000ULL) << 24) |
        ((v & 0x00000000FF000000ULL) <<  8) |
        ((v & 0x000000FF00000000ULL) >>  8) |
        ((v & 0x0000FF0000000000ULL) >> 24) |
        ((v & 0x00FF000000000000ULL) >> 40) |
        ((v & 0xFF00000000000000ULL) >> 56);
#endif
    return v;
}

static std::string
ComputeDerivedDataHash(std::string const &path)
{
    /*
     * This algorithm is documented here:
     * https://samdmarshall.com/blog/xcode_deriveddata_hashes.html
     */

    md5_state_t state;
    md5_init(&state);
    md5_append(&state, reinterpret_cast<const md5_byte_t *>(path.data()), path.size());

    uint8_t digest[16];
    md5_finish(&state, reinterpret_cast<md5_byte_t *>(&digest));

    char hash_path[28];
    int counter;

    uint64_t first_value = hton64(*reinterpret_cast<uint64_t *>(&digest[0]));
    counter = 13;
    while (counter >= 0) {
        hash_path[counter] = 'a' + (first_value % 26);
        first_value /= 26;
        counter--;
    }

    uint64_t second_value = hton64(*reinterpret_cast<uint64_t *>(&digest[8]));
    counter = 27;
    while (counter > 13) {
        hash_path[counter] = 'a' + (second_value % 26);
        second_value /= 26;
        counter--;
    }

    return std::string(hash_path, 28);
}

DerivedDataHash DerivedDataHash::
Create(std::string const &path)
{
    return DerivedDataHash(
        FSUtil::GetBaseNameWithoutExtension(path),
        ComputeDerivedDataHash(path));
}
