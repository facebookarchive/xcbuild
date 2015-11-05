/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_Crypto_h
#define __libutil_Crypto_h

#include <libutil/Base.h>

namespace libutil {

class Crypto {
public:
    static bool GetRandom(size_t count, void *buffer);
};

}

#endif  // !__libutil_Crypto_h
