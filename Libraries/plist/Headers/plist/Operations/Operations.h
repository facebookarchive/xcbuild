/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __plist_Operations_h
#define __plist_Operations_h

#include <libutil/Filesystem.h>
#include <iterator>

namespace plist {

std::pair<bool, std::vector<uint8_t>>
Read(libutil::Filesystem const *filesystem, std::string const &path = "-");

bool
Write(libutil::Filesystem *filesystem, std::vector<uint8_t> const &contents, std::string const &path = "-");

}

#endif  // !__plist_Operations_h
