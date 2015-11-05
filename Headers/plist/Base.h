/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Base_h
#define __plist_Base_h

#include <libutil/libutil.h>

namespace plist {

using string_vector = libutil::string_vector;
using string_map = libutil::string_map;

//
// For non-binary plist, first two arguments are line and column
// numbers, otherwise they're zero.
//
using error_function = std::function <void(unsigned, unsigned, std::string const &)>;

}

#endif  // !__plist_Base_h
