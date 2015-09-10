// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_HeaderMap_h
#define __pbxbuild_HeaderMap_h

#include <pbxbuild/HMapFile.h>

namespace pbxbuild {

class HeaderMap {
private:
    typedef std::map <std::string, size_t> string_offset_map;

private:
    HMapHeader               _header;
    std::vector <HMapBucket> _buckets;
    std::vector <char>       _strings;
    std::set <std::string>   _keys;
    string_offset_map        _offsets;
    bool                     _modified;

public:
    HeaderMap();

public:
    bool read(std::vector<char> const &buffer);
    bool write(std::vector<char> *buffer);

public:
    void invalidate();

public:
    bool add(std::string const &key, std::string const &prefix,
            std::string const &suffix);

public:
    void dump();

private:
    void grow();
    void rehash(uint32_t newNumBuckets);
    void set(unsigned hash, uint32_t koff, uint32_t poff, uint32_t soff, bool growing);
    string_offset_map::iterator add(std::string const &string, bool key);
};

}

#endif  // !__pbxbuild_HeaderMap_h
