// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_Base64_h
#define __libutil_Base64_h

#include <libutil/Base.h>

namespace libutil {

struct Base64 {
    static void Decode(std::string const &in, std::vector <uint8_t> &out);
    static std::string Encode(std::vector <uint8_t> const &in);
};

}

#endif  // !__libutil_Base64_h
