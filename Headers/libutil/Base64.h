/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
