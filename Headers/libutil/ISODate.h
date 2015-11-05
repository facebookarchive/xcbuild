/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
