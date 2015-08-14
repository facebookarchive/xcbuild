// Copyright 2013-present Facebook. All Rights Reserved.

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
