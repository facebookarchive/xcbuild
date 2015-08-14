// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/HeaderMap.h>

using pbxbuild::HeaderMap;
using libutil::FSUtil;

HeaderMap::HeaderMap() :
    _modified(false)
{
    std::memset(&_header, 0, sizeof(_header));

    //
    // Initially we set to 8 buckets.
    //
    _header.NumBuckets = 8;
    _buckets.resize(_header.NumBuckets);
}

bool HeaderMap::
read(std::string const &path)
{
    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if (fp == nullptr)
        return false;

    std::fseek(fp, 0, SEEK_END);
    long size = std::ftell(fp);
    std::rewind(fp);

    if (std::fread(&_header, 1, sizeof(HMapHeader), fp) != sizeof(HMapHeader)) {
        std::fclose(fp);
        return false;
    }

    // 
    // TODO Reverse endian
    //
    if (_header.Magic != HMAP_HeaderMagicNumber ||
        _header.Version != HMAP_HeaderVersion ||
        _header.Reserved != 0 ||
        _header.StringsOffset > static_cast <uint32_t> (size)) {
        invalidate();
        std::fclose(fp);
        return false;
    }
    
    //
    // Reset
    //
    _offsets.clear();
    _keys.clear();

    //
    // Read buckets and strings
    //
    _buckets.resize(_header.NumBuckets);
    if (std::fread(&_buckets[0], sizeof(HMapBucket),
                _header.NumBuckets, fp) != _header.NumBuckets) {
        invalidate();
        std::fclose(fp);
        return false;
    }

    size_t stringSize = size - _header.StringsOffset;
    std::fseek(fp, _header.StringsOffset, SEEK_SET);
    _strings.resize(stringSize);
    if (std::fread(&_strings[0], 1, _strings.size(), fp) != _strings.size()) {
        invalidate();
        std::fclose(fp);
        return false;
    }

    //
    // Now fill _keys and _offsets so that we can manipulate this hmap.
    //
    for (size_t n = 0; n < _buckets.size(); n++) {
        if (_buckets[n].Key == HMAP_EmptyBucketKey)
            continue;

        if (_buckets[n].Key >= _strings.size() ||
            _buckets[n].Suffix >= _strings.size() ||
            _buckets[n].Prefix >= _strings.size())
            continue;

        _keys.insert(&_strings[_buckets[n].Key]);
        _offsets.insert(std::make_pair(
                    &_strings[_buckets[n].Key],
                    _buckets[n].Key));
        _offsets.insert(std::make_pair(
                    &_strings[_buckets[n].Prefix],
                    _buckets[n].Prefix));
        _offsets.insert(std::make_pair(
                    &_strings[_buckets[n].Suffix],
                    _buckets[n].Suffix));
    }

    std::fclose(fp);

    _modified = false;

    return true;
}

bool HeaderMap::
write(std::string const &path)
{
    std::FILE *fp = std::fopen(path.c_str(), "wb");
    if (fp == nullptr)
        return false;

    if (_modified) {
        rehash(_header.NumBuckets);
    }

    _header.Magic         = HMAP_HeaderMagicNumber;
    _header.Version       = HMAP_HeaderVersion;
    _header.Reserved      = 0;
    _header.StringsOffset = sizeof(_header) + _header.NumBuckets * sizeof(HMapBucket);

    if (std::fwrite(&_header, 1, sizeof(HMapHeader), fp) != sizeof(HMapHeader)) {
        std::fclose(fp);
        FSUtil::Remove(path);
        return false;
    }

    //
    // Write buckets and strings
    //
    if (std::fwrite(&_buckets[0], sizeof(HMapBucket),
                _header.NumBuckets, fp) != _header.NumBuckets) {
        std::fclose(fp);
        FSUtil::Remove(path);
        return false;
    }

    if (std::fwrite(&_strings[0], 1, _strings.size(), fp) != _strings.size()) {
        std::fclose(fp);
        FSUtil::Remove(path);
        return false;
    }

    std::fclose(fp);
    return true;
}

void HeaderMap::
invalidate()
{
    std::memset(&_header, 0, sizeof(_header));
    _buckets.clear();
    _strings.clear();
    _offsets.clear();
    _keys.clear();
    _modified = false;
}

bool HeaderMap::
add(std::string const &key, std::string const &prefix,
        std::string const &suffix)
{
    if (key.empty() || prefix.empty() || suffix.empty())
        return false; // invalid argument

    if (_keys.find(key) != _keys.end())
        return false; // already exists

    //
    // Lookup key offset, if none, add one.
    //
    auto KI = add(key, true);

    //
    // Lookup prefix offset, if none, add one.
    //
    auto PI = add(prefix, false);

    //
    // Lookup suffix offset, if none, add one.
    //
    auto SI = add(suffix, false);

    //
    // Now let the table grow by one item, if needed.
    //
    grow();

    //
    // Find the bucket for this key.
    //
    unsigned hash = HashHMapKey(key) % _header.NumBuckets;
    set(hash, KI->second, PI->second, SI->second, false);
    if (key.length() > _header.MaxValueLength) {
        _header.MaxValueLength = key.length();
    }

    _modified = true;

    return true;
}

void HeaderMap::
grow()
{
    //
    // Grow at 3/4 of buckets
    //
    if (_header.NumEntries + 1 >= (_header.NumBuckets * 3) / 4) {
        //
        // Resize and rehash
        //
        rehash(_header.NumBuckets << 1);
    }
}

void HeaderMap::
rehash(uint32_t newNumBuckets)
{
    std::vector <HMapBucket> buckets;

    buckets.resize(newNumBuckets);
    _buckets.swap(buckets);

    std::multimap <uint32_t, HMapBucket *> rehashed;

    for (size_t n = 0; n < _header.NumBuckets; n++) {
        if (buckets[n].Key == HMAP_EmptyBucketKey)
            continue;

        unsigned hash = HashHMapKey(&_strings[buckets[n].Key]) % newNumBuckets;
        rehashed.insert(std::make_pair(hash, &buckets[n]));
    }

    for (auto BI : rehashed) {
        set(BI.first, BI.second->Key, BI.second->Prefix, BI.second->Suffix, true);
    }

    _header.NumBuckets = newNumBuckets;

    _modified = false;
}

HeaderMap::string_offset_map::iterator HeaderMap::
add(std::string const &string, bool key)
{
    auto I = _offsets.find(string);
    if (I == _offsets.end()) {
        //
        // Since 0 is reserved, if the strings table is empty,
        // add an empty slot.
        //
        if (_strings.empty()) {
            _strings.resize(1);
        }

        size_t offset = _strings.size();
        _strings.resize(offset + string.length() + 1);
        std::memcpy(&_strings[offset], &string[0], string.length() + 1);

        I = _offsets.insert(std::make_pair(string, offset)).first;

        if (key) {
            _keys.insert(string);
        }
    }
    return I;
}

void HeaderMap::
set(unsigned hash, uint32_t koff, uint32_t poff, uint32_t soff, bool growing)
{
    for (size_t n = hash; n != (hash - 1); n = (n + 1) % _header.NumBuckets) {
        if (_buckets[n].Key == HMAP_EmptyBucketKey) {
            _buckets[n].Key    = koff;
            _buckets[n].Prefix = poff;
            _buckets[n].Suffix = soff;
            if (!growing) {
                _header.NumEntries++;
            }
            break;
        }
    }
}

void HeaderMap::
dump()
{
    fprintf(stderr, "Num Entries = %u Num Buckets = %u Strings Offset = %#x\n",
            _header.NumEntries, _header.NumBuckets, _header.StringsOffset);

    for (size_t n = 0; n < _buckets.size(); n++) {
        if (_buckets[n].Key == HMAP_EmptyBucketKey)
            continue;

        if (_buckets[n].Key >= _strings.size() ||
            _buckets[n].Suffix >= _strings.size() ||
            _buckets[n].Prefix >= _strings.size()) {
            fprintf(stderr, "Bucket #%zu: broken\n", n);
        } else {
            fprintf(stderr,
                    "Bucket #%zu: [%u] Key = '%s' Prefix = '%s' Suffix = '%s'\n",
                    n, HashHMapKey(&_strings[_buckets[n].Key]) % _header.NumBuckets,
                    &_strings[_buckets[n].Key], &_strings[_buckets[n].Prefix],
                    &_strings[_buckets[n].Suffix]);
        }
    }
}
