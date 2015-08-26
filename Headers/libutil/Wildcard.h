// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __libutil_Wildcard_h
#define __libutil_Wildcard_h

#include <libutil/Base.h>

namespace libutil {

struct Wildcard {
    static bool Match(std::string const &pattern, std::string const &string);
};

}

#endif  // !__libutil_Wildcard_h
