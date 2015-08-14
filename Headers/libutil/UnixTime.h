// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_UnixTime_h
#define __libutil_UnixTime_h

#include <libutil/Base.h>

namespace libutil {

struct UnixTime {
    static void Decode(uint64_t in, struct tm &out);
    static uint64_t Encode(struct tm const &in);
};

}

#endif  // !__libutil_UnixTime_h
