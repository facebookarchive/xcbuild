// Copyright 2013-present Facebook. All Rights Reserved.

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
