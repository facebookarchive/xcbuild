// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_ISODate_h
#define __libutil_ISODate_h

#include <libutil/Base.h>

namespace libutil {

struct ISODate {
    static void Decode(std::string const &in, struct tm &out);
    static std::string Encode(struct tm const &in);
};

}

#endif  // !__libutil_ISODate_h
